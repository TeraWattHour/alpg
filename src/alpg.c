#include <stdio.h>
#include <postgres.h>
#include <fmgr.h>
#include <funcapi.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <utils/syscache.h>
#include <utils/guc.h>
#include <utils/jsonb.h>
#include "utils/builtins.h"

#include "curl_helpers.c"
#include "print_helpers.c"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(search_algolia);

Datum search_algolia(PG_FUNCTION_ARGS) {
    FuncCallContext *ctx;
    TupleDesc tuple_desc;
    CURL *curl;

    if (SRF_IS_FIRSTCALL()) {
        ctx = SRF_FIRSTCALL_INIT();
        MemoryContext old_ctx = MemoryContextSwitchTo(ctx->multi_call_memory_ctx);

        tuple_desc = CreateTemplateTupleDesc(1);
        TupleDescInitEntry(tuple_desc, 1, "document", JSONBOID, -1, 0);
        tuple_desc = BlessTupleDesc(tuple_desc);

        if (get_call_result_type(fcinfo, NULL, &tuple_desc) != TYPEFUNC_COMPOSITE) {
            ereport(ERROR, errcode(ERRCODE_FEATURE_NOT_SUPPORTED), errmsg("function returning record called in context that cannot accept type record"));
        }

        curl = curl_easy_init();
        if (curl == NULL) {
            ereport(ERROR, errcode(ERRCODE_INTERNAL_ERROR), errmsg("failed to set up libcurl"));
        }

        // reading environment variables
        char *api_key = GetConfigOptionByName("alpg.api_key", NULL, false);
        char *app_id = GetConfigOptionByName("alpg.app_id", NULL, false);

        // reading arguments from function context
        char *index_name = text_to_cstring(PG_GETARG_TEXT_PP(0));
        char *search_string = text_to_cstring(PG_GETARG_TEXT_PP(1));
        char *attributes_to_retrieve = text_to_cstring(PG_GETARG_TEXT_PP(2));
        int32 page = !PG_ARGISNULL(3) ? PG_GETARG_INT32(3) : 0;
        int32 per_page = !PG_ARGISNULL(4) ? PG_GETARG_INT32(4) : 30;

        struct curl_slist *header_list = NULL;
        header_list = curl_slist_append(header_list, psprintf("X-Algolia-API-Key: %s", api_key));
        header_list = curl_slist_append(header_list, psprintf("X-Algolia-Application-Id: %s", app_id));


        char *url = psprintf("https://%s-dsn.algolia.net/1/indexes/%s", app_id, index_name);

        CURLU *h = curl_url();
        curl_url_set(h, CURLUPART_URL, url, 0);
        curl_url_set(h, CURLUPART_QUERY, psprintf("page=%d", page), CURLU_APPENDQUERY);
        curl_url_set(h, CURLUPART_QUERY, psprintf("hitsPerPage=%d", per_page), CURLU_APPENDQUERY);
        curl_url_set(h, CURLUPART_QUERY, psprintf("attributesToRetrieve=%s", attributes_to_retrieve), CURLU_APPENDQUERY | CURLU_URLENCODE);
        curl_url_set(h, CURLUPART_QUERY, "attributesToHighlight=", CURLU_APPENDQUERY);
        curl_url_set(h, CURLUPART_QUERY, "responseFields=hits", CURLU_APPENDQUERY);
        curl_url_set(h, CURLUPART_QUERY, psprintf("query=%s", search_string), CURLU_APPENDQUERY | CURLU_URLENCODE);

        CurlResponse response_data = init_curl_response();

        curl_easy_setopt(curl, CURLOPT_CURLU, h);
        curl_easy_setopt(curl, CURLOPT_HTTPGET, true);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_generic_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response_data);

        CURLcode curl_code = curl_easy_perform(curl);

        long http_code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (curl_code != CURLE_OK) {
            ereport(ERROR, errcode(ERRCODE_SYSTEM_ERROR), errmsg("algolia search request failed"));
        }

        cJSON *json = cJSON_Parse(response_data.content);

        const char *err = cJSON_GetErrorPtr();
        if (err != NULL)
        {
            ereport(ERROR, errcode(ERRCODE_SYSTEM_ERROR), errmsg("algolia search request returned malformed data"));
        }

        if (http_code != 200) {
            cJSON *message = cJSON_GetObjectItem(json, "message");
            if (message != NULL && strlen(message->valuestring) > 0) {
                ereport(ERROR, errcode(ERRCODE_SYSTEM_ERROR),
                        errmsg("algolia search request returned an error: %s", message->valuestring));
            }
            ereport(ERROR, errcode(ERRCODE_SYSTEM_ERROR),
                    errmsg("algolia search request returned an unknown error"));
        }

        cJSON *hits = cJSON_GetObjectItem(json, "hits");
        cJSON *hit;

        int hit_count = cJSON_GetArraySize(hits);

        Jsonb **results = palloc(sizeof(Jsonb *) * hit_count);

        int i = 0;
        for(hit = (hits != NULL) ? (hits)->child : NULL; hit != NULL; hit = hit->next, i++) {
            char *document = cJSON_PrintUnformatted(hit);

            results[i] = DatumGetJsonbP(DirectFunctionCall1(jsonb_in, CStringGetDatum(document)));
        }

        curl_slist_free_all(header_list);
        curl_easy_cleanup(curl);

        cJSON_Delete(json);

        ctx->user_fctx = results;
        ctx->max_calls = hit_count;
        ctx->tuple_desc = tuple_desc;

        MemoryContextSwitchTo(old_ctx);
    }

    ctx = SRF_PERCALL_SETUP();

    if (ctx->call_cntr < ctx->max_calls) {
        Jsonb *current_value = ((Jsonb **)ctx->user_fctx)[ctx->call_cntr];

        Datum values[] = { JsonbPGetDatum(current_value) };
        bool nulls[] = { false };

        HeapTuple tuple = heap_form_tuple(ctx->tuple_desc, values, nulls);

        SRF_RETURN_NEXT(ctx, HeapTupleGetDatum(tuple));
    } else {
        SRF_RETURN_DONE(ctx);
    }
}