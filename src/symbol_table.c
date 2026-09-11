#include "../include/white/symbol_table.h"
#include "../include/white/common.h"
#include <string.h>

SymbolTable *symbol_table_create(void) {
    SymbolTable *table = xmalloc(sizeof(SymbolTable));
    table->symbols = array_create();
    table->parent = NULL;
    return table;
}

SymbolTable *symbol_table_create_child(SymbolTable *parent) {
    SymbolTable *child = symbol_table_create();
    child->parent = parent;
    return child;
}

void symbol_table_define(SymbolTable *table, const char *name, WhiteType *type, int kind) {
    Symbol *sym = xmalloc(sizeof(Symbol));
    sym->name = str_dup(name);
    sym->type = type;
    sym->kind = kind;
    sym->is_defined = true;
    sym->is_initialized = (kind == SYMBOL_VAR_CONST);
    array_push(table->symbols, sym);
}

Symbol *symbol_table_lookup(SymbolTable *table, const char *name) {
    if (!table) return NULL;
    
    /* Search in current scope */
    for (size_t i = 0; i < table->symbols->count; i++) {
        Symbol *sym = (Symbol *)array_get(table->symbols, i);
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
    }
    
    /* Search in parent scope */
    if (table->parent) {
        return symbol_table_lookup(table->parent, name);
    }
    
    return NULL;
}

Symbol *symbol_table_lookup_local(SymbolTable *table, const char *name) {
    if (!table) return NULL;
    
    for (size_t i = 0; i < table->symbols->count; i++) {
        Symbol *sym = (Symbol *)array_get(table->symbols, i);
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
    }
    
    return NULL;
}

void symbol_table_free(SymbolTable *table) {
    if (!table) return;
    
    for (size_t i = 0; i < table->symbols->count; i++) {
        Symbol *sym = (Symbol *)array_get(table->symbols, i);
        xfree(sym->name);
        if (sym->type) type_free(sym->type);
        xfree(sym);
    }
    
    array_free(table->symbols);
    xfree(table);
}
