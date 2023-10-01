//
// Created by terawatthour on 28.09.23.
//

typedef struct curl_response {
    char *content;
    size_t size;
} CurlResponse;

CurlResponse init_curl_response() {
    CurlResponse data = { .content = "", .size = 0 };
    return data;
}

static size_t curl_generic_callback(void *data, size_t size, size_t nmemb, CurlResponse *mem) {
    size_t sz = size * nmemb;

    if(mem->content == NULL || mem->size <= 0) {
        mem->content = palloc(sz + 1);
    } else {
        mem->content = repalloc(mem->content, mem->size + sz + 1);
    }

    if(mem->content == NULL)
        return 0;

    memcpy(&(mem->content[mem->size]), data, sz);
    mem->size += sz;
    mem->content[mem->size] = 0;

    return sz;
}