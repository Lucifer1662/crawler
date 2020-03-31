#include <ctype.h>

#include "stdarg.h"
#include "stdlib.h"
#include "string.h"

char* concat_create(int count, ...) {
    va_list argp;
    int len = 1;
    int temp_count = count;
    va_start(argp, count);
    for (int i = 0; i < count; i++) {
        len += strlen(va_arg(argp, char*));
    }
    va_end(argp);

    char* str = (char*)malloc(len);
    memset(str, 0, len);
    count = temp_count;
    va_start(argp, count);
    for (int i = 0; i < count; i++) {
        strcat(str, va_arg(argp, char*));
    }
    va_end(argp);
    return str;
}

void str_to_lower(char* str) {
    while (*str) {
        *str = tolower(*str);
        str++;
    }
}