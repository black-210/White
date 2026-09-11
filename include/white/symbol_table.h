#ifndef WHITE_SYMBOL_TABLE_H
#define WHITE_SYMBOL_TABLE_H

#include "types.h"
#include "common.h"

#define SYMBOL_VAR_LET    1
#define SYMBOL_VAR_CONST  2
#define SYMBOL_FUNCTION   3
#define SYMBOL_STRUCT     4
#define SYMBOL_ENUM       5
#define SYMBOL_TYPE_ALIAS 6

typedef struct {
    char *name;
    WhiteType *type;
    int kind;
    bool is_defined;
    bool is_initialized;
    int line;
    int column;
} Symbol;

typedef struct SymbolTable {
    DynamicArray *symbols;
    struct SymbolTable *parent;
} SymbolTable;

SymbolTable *symbol_table_create(void);
SymbolTable *symbol_table_create_child(SymbolTable *parent);
void symbol_table_define(SymbolTable *table, const char *name, WhiteType *type, int kind);
Symbol *symbol_table_lookup(SymbolTable *table, const char *name);
Symbol *symbol_table_lookup_local(SymbolTable *table, const char *name);
void symbol_table_free(SymbolTable *table);

#endif
