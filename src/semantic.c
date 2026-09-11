#include "../include/white/semantic.h"
#include <string.h>

SemanticAnalyzer *semantic_create(void) {
    SemanticAnalyzer *sa = xmalloc(sizeof(SemanticAnalyzer));
    sa->scopes = array_create();
    sa->functions = array_create();
    sa->errors = array_create();
    sa->scope_level = 0;
    return sa;
}

void semantic_analyze(SemanticAnalyzer *sa, ASTNode *program) {
    if (!program || program->type != AST_PROGRAM) return;
    
    ASTProgram *prog = (ASTProgram *)program->data;
    
    for (size_t i = 0; i < prog->statements->count; i++) {
        ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
        if (stmt && stmt->type == AST_FUNC_DEF) {
            array_push(sa->functions, stmt);
        }
    }
}

void semantic_free(SemanticAnalyzer *sa) {
    array_free(sa->scopes);
    array_free(sa->functions);
    array_free(sa->errors);
    xfree(sa);
}

WhiteType *semantic_get_expr_type(SemanticAnalyzer *sa, ASTNode *expr) {
    if (!expr) return type_create_primitive(TYPE_VOID);
    
    switch (expr->type) {
        case AST_INT_LITERAL:
            return type_create_primitive(TYPE_I64);
        case AST_FLOAT_LITERAL:
            return type_create_primitive(TYPE_F64);
        case AST_STRING_LITERAL:
            return type_create_primitive(TYPE_STRING);
        case AST_BOOL_LITERAL:
            return type_create_primitive(TYPE_BOOL);
        default:
            return type_create_primitive(TYPE_UNKNOWN);
    }
}

bool semantic_check_type_compat(WhiteType *from, WhiteType *to) {
    if (!from || !to) return true;
    if (type_equals(from, to)) return true;
    
    /* Allow implicit conversions for numeric types */
    if (type_is_numeric(from) && type_is_numeric(to)) return true;
    
    return false;
}
