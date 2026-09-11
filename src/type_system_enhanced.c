#include "../include/white/types.h"
#include "../include/white/common.h"
#include <string.h>
#include <stdio.h>

WhiteType *type_create_primitive(TypeKind kind) {
    WhiteType *t = xmalloc(sizeof(WhiteType));
    t->kind = kind;
    t->name = NULL;
    t->element_type = NULL;
    t->array_size = 0;
    t->pointed_type = NULL;
    t->fields = NULL;
    t->field_types = NULL;
    t->return_type = NULL;
    t->param_types = NULL;
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
    t->fields = array_create();
    t->field_types = array_create();
    return t;
}

WhiteType *type_create_function(WhiteType *return_type, DynamicArray *params) {
    WhiteType *t = type_create_primitive(TYPE_FUNCTION);
    t->return_type = return_type;
    t->param_types = params;
    return t;
}

bool type_equals(WhiteType *a, WhiteType *b) {
    if (!a || !b) return false;
    if (a->kind != b->kind) return false;
    
    if (a->kind == TYPE_ARRAY) {
        return type_equals(a->element_type, b->element_type) && a->array_size == b->array_size;
    }
    
    if (a->kind == TYPE_POINTER) {
        return type_equals(a->pointed_type, b->pointed_type);
    }
    
    if (a->kind == TYPE_STRUCT) {
        return strcmp(a->name, b->name) == 0;
    }
    
    return true;
}

bool type_is_numeric(WhiteType *t) {
    if (!t) return false;
    return type_is_integer(t) || type_is_float(t);
}

bool type_is_integer(WhiteType *t) {
    if (!t) return false;
    return t->kind >= TYPE_I8 && t->kind <= TYPE_U64;
}

bool type_is_float(WhiteType *t) {
    if (!t) return false;
    return t->kind == TYPE_F32 || t->kind == TYPE_F64;
}

bool type_is_rf_type(WhiteType *t) {
    if (!t) return false;
    return t->kind >= TYPE_COMPLEX && t->kind <= TYPE_SPECTRUM;
}

const char *type_to_string(WhiteType *t) {
    if (!t) return "unknown";
    
    static char buffer[512];
    
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
        case TYPE_ARRAY:
            snprintf(buffer, sizeof(buffer), "[%s; %zu]", type_to_string(t->element_type), t->array_size);
            return buffer;
        case TYPE_POINTER:
            snprintf(buffer, sizeof(buffer), "*%s", type_to_string(t->pointed_type));
            return buffer;
        case TYPE_STRUCT:
            return t->name;
        default:
            return "unknown";
    }
}

const char *type_to_c_type(WhiteType *t) {
    if (!t) return "void";
    
    static char buffer[512];
    
    switch (t->kind) {
        case TYPE_VOID: return "void";
        case TYPE_BOOL: return "_Bool";
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
        case TYPE_COMPLEX: return "double complex";
        case TYPE_FREQUENCY: return "double";
        case TYPE_SAMPLE_RATE: return "double";
        case TYPE_BANDWIDTH: return "double";
        case TYPE_AMPLITUDE: return "double";
        case TYPE_PHASE: return "double";
        case TYPE_POWER: return "double";
        case TYPE_IQ: return "white_iq";
        case TYPE_SIGNAL: return "white_signal*";
        case TYPE_SPECTRUM: return "white_spectrum*";
        case TYPE_ARRAY:
            snprintf(buffer, sizeof(buffer), "%s*", type_to_c_type(t->element_type));
            return buffer;
        case TYPE_POINTER:
            snprintf(buffer, sizeof(buffer), "%s*", type_to_c_type(t->pointed_type));
            return buffer;
        case TYPE_STRUCT:
            return t->name;
        default:
            return "void";
    }
}

void type_free(WhiteType *t) {
    if (!t) return;
    
    if (t->name) xfree(t->name);
    if (t->element_type) type_free(t->element_type);
    if (t->pointed_type) type_free(t->pointed_type);
    if (t->return_type) type_free(t->return_type);
    
    if (t->fields) {
        for (size_t i = 0; i < t->fields->count; i++) {
            xfree(array_get(t->fields, i));
        }
        array_free(t->fields);
    }
    
    if (t->field_types) {
        for (size_t i = 0; i < t->field_types->count; i++) {
            type_free((WhiteType *)array_get(t->field_types, i));
        }
        array_free(t->field_types);
    }
    
    if (t->param_types) {
        for (size_t i = 0; i < t->param_types->count; i++) {
            type_free((WhiteType *)array_get(t->param_types, i));
        }
        array_free(t->param_types);
    }
    
    xfree(t);
}
