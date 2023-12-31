//
// Created by terawatthour on 28.09.23.
//

#include "curl_helpers.h"

CurlResponse init_curl_response() {
    CurlResponse data = { .content = palloc(0), .size = 0 };
    return data;
}

size_t curl_generic_callback(void *data, size_t size, size_t nmemb, CurlResponse *mem) {
    size_t sz = size * nmemb;

    mem->content = repalloc(mem->content, mem->size + sz + 1);
    if(mem->content == NULL)
        return 0;

    memcpy(&(mem->content[mem->size]), data, sz);
    mem->size += sz;
    mem->content[mem->size] = '\0';

    return sz;
}