//
// Created by terawatthour on 08/10/2023.
//

#ifndef ALPG_CURL_HELPERS_H
#define ALPG_CURL_HELPERS_H

#include <postgres.h>

typedef struct curl_response {
    char *content;
    size_t size;
} CurlResponse;

CurlResponse init_curl_response();

size_t curl_generic_callback(void *data, size_t size, size_t nmemb, CurlResponse *mem);

#endif //ALPG_CURL_HELPERS_H
