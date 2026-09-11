#include "../include/white/common.h"
#include <stdarg.h>
#include <ctype.h>

int white_string_length(const char *str) {
    return str ? strlen(str) : 0;
}

char *white_string_substring(const char *str, int start, int length) {
    if (!str || start < 0 || length <= 0) return str_dup("");
    int str_len = strlen(str);
    if (start >= str_len) return str_dup("");
    if (start + length > str_len) length = str_len - start;
    
    char *result = xmalloc(length + 1);
    strncpy(result, str + start, length);
    result[length] = 0;
    return result;
}

char *white_string_upper(const char *str) {
    if (!str) return str_dup("");
    char *result = str_dup(str);
    for (int i = 0; result[i]; i++) {
        result[i] = toupper((unsigned char)result[i]);
    }
    return result;
}

char *white_string_lower(const char *str) {
    if (!str) return str_dup("");
    char *result = str_dup(str);
    for (int i = 0; result[i]; i++) {
        result[i] = tolower((unsigned char)result[i]);
    }
    return result;
}

int white_string_find(const char *str, const char *substr) {
    if (!str || !substr) return -1;
    char *pos = strstr(str, substr);
    return pos ? (pos - str) : -1;
}

char *white_string_replace(const char *str, const char *from, const char *to) {
    if (!str || !from || !to) return str_dup(str);
    
    size_t from_len = strlen(from);
    size_t to_len = strlen(to);
    size_t str_len = strlen(str);
    
    DynamicArray *parts = array_create();
    const char *current = str;
    
    while (*current) {
        char *match = strstr(current, from);
        if (!match) {
            array_push(parts, str_dup(current));
            break;
        }
        
        size_t len = match - current;
        if (len > 0) {
            char *part = xmalloc(len + 1);
            strncpy(part, current, len);
            part[len] = 0;
            array_push(parts, part);
        }
        array_push(parts, str_dup(to));
        current = match + from_len;
    }
    
    size_t total = 0;
    for (size_t i = 0; i < parts->count; i++) {
        total += strlen((char *)array_get(parts, i));
    }
    
    char *result = xmalloc(total + 1);
    result[0] = 0;
    for (size_t i = 0; i < parts->count; i++) {
        strcat(result, (char *)array_get(parts, i));
        xfree(array_get(parts, i));
    }
    array_free(parts);
    
    return result;
}
