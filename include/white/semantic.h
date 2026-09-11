#ifndef WHITE_SEMANTIC_H
#define WHITE_SEMANTIC_H

#include "common.h"
#include "ast.h"
#include "types.h"

typedef struct {
    char *name;
    WhiteType *type;
    bool is_const;
    int scope_level;
} Symbol;

typedef struct {
    DynamicArray *scopes;  /* Stack of symbol tables */
    DynamicArray *functions;  /* Function definitions */
    DynamicArray *errors;
    int scope_level;
} SemanticAnalyzer;

SemanticAnalyzer *semantic_create(void);
void semantic_analyze(SemanticAnalyzer *sa, ASTNode *program);
void semantic_free(SemanticAnalyzer *sa);

WhiteType *semantic_get_expr_type(SemanticAnalyzer *sa, ASTNode *expr);
bool semantic_check_type_compat(WhiteType *from, WhiteType *to);

#endif
