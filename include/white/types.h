#ifndef WHITE_TYPES_H
#define WHITE_TYPES_H

#include "common.h"

typedef enum {
    TYPE_VOID,
    TYPE_BOOL,
    TYPE_I8,
    TYPE_I16,
    TYPE_I32,
    TYPE_I64,
    TYPE_U8,
    TYPE_U16,
    TYPE_U32,
    TYPE_U64,
    TYPE_F32,
    TYPE_F64,
    TYPE_STRING,
    TYPE_COMPLEX,
    TYPE_FREQUENCY,
    TYPE_SAMPLE_RATE,
    TYPE_BANDWIDTH,
    TYPE_AMPLITUDE,
    TYPE_PHASE,
    TYPE_POWER,
    TYPE_IQ,
    TYPE_SIGNAL,
    TYPE_SPECTRUM,
    TYPE_ARRAY,
    TYPE_POINTER,
    TYPE_STRUCT,
    TYPE_ENUM,
    TYPE_FUNCTION,
    TYPE_UNKNOWN,
} TypeKind;

typedef struct WhiteType {
    TypeKind kind;
    char *name;
    /* For arrays */
    struct WhiteType *element_type;
    size_t array_size;
    /* For pointers */
    struct WhiteType *pointed_type;
    /* For structs/enums */
    DynamicArray *fields;  /* field names */
    DynamicArray *field_types;  /* field types */
    /* For functions */
    struct WhiteType *return_type;
    DynamicArray *param_types;
} WhiteType;

WhiteType *type_create_primitive(TypeKind kind);
WhiteType *type_create_array(WhiteType *element_type, size_t size);
WhiteType *type_create_pointer(WhiteType *pointed_type);
WhiteType *type_create_struct(const char *name);
WhiteType *type_create_function(WhiteType *return_type, DynamicArray *params);

bool type_equals(WhiteType *a, WhiteType *b);
bool type_is_numeric(WhiteType *t);
bool type_is_integer(WhiteType *t);
bool type_is_float(WhiteType *t);
bool type_is_rf_type(WhiteType *t);

const char *type_to_string(WhiteType *t);
const char *type_to_c_type(WhiteType *t);

void type_free(WhiteType *t);

#endif
