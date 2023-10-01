//
// Created by terawatthour on 28.09.23.
//

#include <postgres.h>
#include <stdio.h>
#include <stdarg.h>

char *vpsprintf(const char *fmt, va_list ap) {
    va_list ap1;

    va_copy(ap1, ap);
    int len = vsnprintf(NULL, 0, fmt, ap1);

    if (len < 0)
        return NULL;

    va_end(ap1);

    char *buffer = palloc(len + 1);
    if (!buffer)
        return NULL;

    vsnprintf(buffer, len + 1, fmt, ap);
    return buffer;
}

char *psprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char *buffer = vpsprintf(fmt, ap);
    va_end(ap);
    return buffer;
}