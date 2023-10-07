#ifndef ALPG_PRINT_HELPERS_H
#define ALPG_PRINT_HELPERS_H

#include <postgres.h>
#include <stdarg.h>

char *vpsprintf(const char *fmt, va_list ap);

char *psprintf(const char *fmt, ...);

#endif //ALPG_PRINT_HELPERS_H