#include "../include/white/common.h"
#include <stdlib.h>
#include <string.h>

void *xmalloc(size_t size) {
    void *ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(1);
    }
    return ptr;
}

void *xcalloc(size_t count, size_t size) {
    void *ptr = calloc(count, size);
    if (!ptr) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(1);
    }
    return ptr;
}

void *xrealloc(void *ptr, size_t size) {
    void *new_ptr = realloc(ptr, size);
    if (!new_ptr) {
        fprintf(stderr, "Fatal: out of memory\n");
        exit(1);
    }
    return new_ptr;
}

void xfree(void *ptr) {
    if (ptr) free(ptr);
}

char *str_dup(const char *str) {
    if (!str) return NULL;
    size_t len = strlen(str);
    char *dup = xmalloc(len + 1);
    strcpy(dup, str);
    return dup;
}

char *str_concat(const char *a, const char *b) {
    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    char *result = xmalloc(len_a + len_b + 1);
    strcpy(result, a);
    strcpy(result + len_a, b);
    return result;
}

bool str_equals(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

bool str_starts_with(const char *str, const char *prefix) {
    return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool str_ends_with(const char *str, const char *suffix) {
    size_t str_len = strlen(str);
    size_t suf_len = strlen(suffix);
    if (suf_len > str_len) return false;
    return strcmp(str + str_len - suf_len, suffix) == 0;
}

DynamicArray *array_create(void) {
    DynamicArray *arr = xmalloc(sizeof(DynamicArray));
    arr->items = NULL;
    arr->count = 0;
    arr->capacity = 0;
    return arr;
}

void array_push(DynamicArray *arr, void *item) {
    if (arr->count >= arr->capacity) {
        arr->capacity = arr->capacity == 0 ? 16 : arr->capacity * 2;
        arr->items = xrealloc(arr->items, arr->capacity * sizeof(void *));
    }
    arr->items[arr->count++] = item;
}

void *array_get(DynamicArray *arr, size_t index) {
    if (index >= arr->count) return NULL;
    return arr->items[index];
}

void array_free(DynamicArray *arr) {
    if (arr) {
        xfree(arr->items);
        xfree(arr);
    }
}

uint32_t hash_string(const char *str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

uint32_t hash_combine(uint32_t a, uint32_t b) {
    return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
}
