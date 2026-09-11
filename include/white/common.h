#ifndef WHITE_COMMON_H
#define WHITE_COMMON_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char *filename;
    int line;
    int column;
} SourceLocation;

typedef struct {
    const char *data;
    size_t length;
} StringView;

typedef struct {
    void **items;
    size_t count;
    size_t capacity;
} DynamicArray;

/* Memory management */
void *xmalloc(size_t size);
void *xcalloc(size_t count, size_t size);
void *xrealloc(void *ptr, size_t size);
void xfree(void *ptr);

/* String utilities */
char *str_dup(const char *str);
char *str_concat(const char *a, const char *b);
bool str_equals(const char *a, const char *b);
bool str_starts_with(const char *str, const char *prefix);
bool str_ends_with(const char *str, const char *suffix);

/* Dynamic arrays */
DynamicArray *array_create(void);
void array_push(DynamicArray *arr, void *item);
void *array_get(DynamicArray *arr, size_t index);
void array_free(DynamicArray *arr);

/* Hashing */
uint32_t hash_string(const char *str);
uint32_t hash_combine(uint32_t a, uint32_t b);

#endif
