#include "../include/white/types.h"
#include <string.h>

WhiteType *type_create_primitive(TypeKind kind) {
    WhiteType *t = xmalloc(sizeof(WhiteType));
    t->kind = kind;
    t->name = NULL;
    t->element_type = NULL;
    t->array_size = 0;
    t->pointed_type = NULL;
    t->fields = array_create();
    t->field_types = array_create();
    t->return_type = NULL;
    t->param_types = array_create();
    return t;
}

WhiteType *type_create_array(WhiteType *element_type, size_t size) {
    WhiteType *t = type_create_primitive(TYPE_ARRAY);
    t->element_type = element_type;
    t->array_size = size;
    return t;
}

WhiteType *type_create_pointer(WhiteType *pointed_type) {
    WhiteType *t = type_create_primitive(TYPE_POINTER);
    t->pointed_type = pointed_type;
    return t;
}

WhiteType *type_create_struct(const char *name) {
    WhiteType *t = type_create_primitive(TYPE_STRUCT);
    t->name = str_dup(name);
    return t;
}

WhiteType *type_create_function(WhiteType *return_type, DynamicArray *params) {
    WhiteType *t = type_create_primitive(TYPE_FUNCTION);
    t->return_type = return_type;
    t->param_types = params ? params : array_create();
    return t;
}

bool type_equals(WhiteType *a, WhiteType *b) {
    if (!a || !b) return a == b;
    if (a->kind != b->kind) return false;
    
    switch (a->kind) {
        case TYPE_ARRAY:
            return a->array_size == b->array_size && type_equals(a->element_type, b->element_type);
        case TYPE_POINTER:
            return type_equals(a->pointed_type, b->pointed_type);
        case TYPE_STRUCT:
            return str_equals(a->name, b->name);
        default:
            return true;
    }
}

bool type_is_numeric(WhiteType *t) {
    if (!t) return false;
    return t->kind >= TYPE_I8 && t->kind <= TYPE_F64;
}

bool type_is_integer(WhiteType *t) {
    if (!t) return false;
    return (t->kind >= TYPE_I8 && t->kind <= TYPE_I64) ||
           (t->kind >= TYPE_U8 && t->kind <= TYPE_U64);
}

bool type_is_float(WhiteType *t) {
    if (!t) return false;
    return t->kind == TYPE_F32 || t->kind == TYPE_F64;
}

bool type_is_rf_type(WhiteType *t) {
    if (!t) return false;
    return t->kind >= TYPE_FREQUENCY && t->kind <= TYPE_SPECTRUM;
}

const char *type_to_string(WhiteType *t) {
    if (!t) return "void";
    
    switch (t->kind) {
        case TYPE_VOID: return "void";
        case TYPE_BOOL: return "bool";
        case TYPE_I8: return "i8";
        case TYPE_I16: return "i16";
        case TYPE_I32: return "i32";
        case TYPE_I64: return "i64";
        case TYPE_U8: return "u8";
        case TYPE_U16: return "u16";
        case TYPE_U32: return "u32";
        case TYPE_U64: return "u64";
        case TYPE_F32: return "f32";
        case TYPE_F64: return "f64";
        case TYPE_STRING: return "string";
        case TYPE_COMPLEX: return "complex";
        case TYPE_FREQUENCY: return "frequency";
        case TYPE_SAMPLE_RATE: return "sample_rate";
        case TYPE_BANDWIDTH: return "bandwidth";
        case TYPE_AMPLITUDE: return "amplitude";
        case TYPE_PHASE: return "phase";
        case TYPE_POWER: return "power";
        case TYPE_IQ: return "iq";
        case TYPE_SIGNAL: return "signal";
        case TYPE_SPECTRUM: return "spectrum";
        case TYPE_POINTER: return "ptr";
        case TYPE_STRUCT: return t->name ? t->name : "struct";
        default: return "unknown";
    }
}

const char *type_to_c_type(WhiteType *t) {
    if (!t) return "void";
    
    switch (t->kind) {
        case TYPE_VOID: return "void";
        case TYPE_BOOL: return "bool";
        case TYPE_I8: return "int8_t";
        case TYPE_I16: return "int16_t";
        case TYPE_I32: return "int32_t";
        case TYPE_I64: return "int64_t";
        case TYPE_U8: return "uint8_t";
        case TYPE_U16: return "uint16_t";
        case TYPE_U32: return "uint32_t";
        case TYPE_U64: return "uint64_t";
        case TYPE_F32: return "float";
        case TYPE_F64: return "double";
        case TYPE_STRING: return "char*";
        case TYPE_COMPLEX: return "double _Complex";
        case TYPE_FREQUENCY: return "double";
        case TYPE_SAMPLE_RATE: return "double";
        case TYPE_BANDWIDTH: return "double";
        case TYPE_AMPLITUDE: return "double";
        case TYPE_PHASE: return "double";
        case TYPE_POWER: return "double";
        case TYPE_IQ: return "white_iq";
        case TYPE_SIGNAL: return "white_signal*";
        case TYPE_SPECTRUM: return "white_spectrum*";
        case TYPE_POINTER: return "void*";
        default: return "void";
    }
}

void type_free(WhiteType *t) {
    if (!t) return;
    if (t->name) xfree(t->name);
    if (t->element_type) type_free(t->element_type);
    if (t->pointed_type) type_free(t->pointed_type);
    if (t->return_type) type_free(t->return_type);
    array_free(t->fields);
    array_free(t->field_types);
    array_free(t->param_types);
    xfree(t);
}
