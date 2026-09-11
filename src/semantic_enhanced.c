#include "../include/white/semantic.h"
#include "../include/white/symbol_table.h"
#include "../include/white/diagnostics.h"
#include <string.h>
#include <stdio.h>

typedef struct {
    SymbolTable *current_scope;
    SymbolTable *global_scope;
    DynamicArray *errors;
    DynamicArray *warnings;
} SemanticAnalyzer;

SemanticAnalyzer *semantic_create(void) {
    SemanticAnalyzer *sa = xmalloc(sizeof(SemanticAnalyzer));
    sa->global_scope = symbol_table_create();
    sa->current_scope = sa->global_scope;
    sa->errors = array_create();
    sa->warnings = array_create();
    return sa;
}

static void semantic_enter_scope(SemanticAnalyzer *sa) {
    SymbolTable *child = symbol_table_create_child(sa->current_scope);
    sa->current_scope = child;
}

static void semantic_exit_scope(SemanticAnalyzer *sa) {
    if (sa->current_scope->parent) {
        sa->current_scope = sa->current_scope->parent;
    }
}

static void semantic_error(SemanticAnalyzer *sa, SourceLocation loc, const char *msg) {
    char *error_msg = xmalloc(512);
    snprintf(error_msg, 512, "%s:%d:%d: error: %s", loc.filename, loc.line, loc.column, msg);
    array_push(sa->errors, error_msg);
}

static void semantic_define_builtin_functions(SemanticAnalyzer *sa) {
    /* Built-in functions */
    WhiteType *float_type = type_create_primitive(TYPE_F64);
    WhiteType *string_type = type_create_primitive(TYPE_STRING);
    
    /* math functions */
    symbol_table_define(sa->global_scope, "sqrt", float_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "sin", float_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "cos", float_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "tan", float_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "abs", float_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "log", float_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "exp", float_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "pow", float_type, SYMBOL_FUNCTION);
    
    /* Signal processing functions */
    WhiteType *signal_type = type_create_primitive(TYPE_SIGNAL);
    WhiteType *spectrum_type = type_create_primitive(TYPE_SPECTRUM);
    
    symbol_table_define(sa->global_scope, "fft", spectrum_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "ifft", signal_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "psd", spectrum_type, SYMBOL_FUNCTION);
    
    /* Physics functions */
    symbol_table_define(sa->global_scope, "wavelength", float_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "frequency_from_wavelength", float_type, SYMBOL_FUNCTION);
    symbol_table_define(sa->global_scope, "path_loss", float_type, SYMBOL_FUNCTION);
}

static bool semantic_check_type_compat(WhiteType *expected, WhiteType *actual) {
    if (!expected || !actual) return false;
    
    /* Exact match */
    if (type_equals(expected, actual)) return true;
    
    /* Numeric type promotion */
    if (type_is_numeric(expected) && type_is_numeric(actual)) {
        /* Allow integer to float promotion */
        if (expected->kind == TYPE_F64 && type_is_integer(actual)) return true;
    }
    
    return false;
}

static WhiteType *semantic_check_expr(SemanticAnalyzer *sa, ASTNode *expr) {
    if (!expr) return NULL;
    
    switch (expr->type) {
        case AST_INT_LITERAL:
            return type_create_primitive(TYPE_I64);
            
        case AST_FLOAT_LITERAL:
            return type_create_primitive(TYPE_F64);
            
        case AST_STRING_LITERAL:
            return type_create_primitive(TYPE_STRING);
            
        case AST_BOOL_LITERAL:
            return type_create_primitive(TYPE_BOOL);
            
        case AST_IDENT: {
            ASTIdent *ident = (ASTIdent *)expr->data;
            Symbol *sym = symbol_table_lookup(sa->current_scope, ident->name);
            if (!sym) {
                char msg[256];
                snprintf(msg, sizeof(msg), "undefined variable '%s'", ident->name);
                semantic_error(sa, expr->loc, msg);
                return NULL;
            }
            return sym->type;
        }
        
        case AST_BINARY_OP: {
            ASTBinaryOp *binop = (ASTBinaryOp *)expr->data;
            WhiteType *left_type = semantic_check_expr(sa, binop->left);
            WhiteType *right_type = semantic_check_expr(sa, binop->right);
            
            if (!left_type || !right_type) return NULL;
            
            /* For arithmetic operations, both must be numeric */
            if (binop->op == TOK_PLUS || binop->op == TOK_MINUS || 
                binop->op == TOK_STAR || binop->op == TOK_SLASH) {
                if (!type_is_numeric(left_type) || !type_is_numeric(right_type)) {
                    semantic_error(sa, expr->loc, "invalid operands for arithmetic operation");
                    return NULL;
                }
                /* Return the wider type */
                if (left_type->kind == TYPE_F64 || right_type->kind == TYPE_F64) {
                    return type_create_primitive(TYPE_F64);
                }
                return left_type;
            }
            
            /* Comparison operators return bool */
            if (binop->op == TOK_EQ || binop->op == TOK_NE || 
                binop->op == TOK_LT || binop->op == TOK_LE || 
                binop->op == TOK_GT || binop->op == TOK_GE) {
                return type_create_primitive(TYPE_BOOL);
            }
            
            /* Logical operators */
            if (binop->op == TOK_AND || binop->op == TOK_OR) {
                return type_create_primitive(TYPE_BOOL);
            }
            
            return left_type;
        }
        
        case AST_CALL: {
            ASTCall *call = (ASTCall *)expr->data;
            ASTIdent *func_ident = (ASTIdent *)call->func->data;
            Symbol *func_sym = symbol_table_lookup(sa->current_scope, func_ident->name);
            
            if (!func_sym) {
                char msg[256];
                snprintf(msg, sizeof(msg), "undefined function '%s'", func_ident->name);
                semantic_error(sa, expr->loc, msg);
                return NULL;
            }
            
            if (func_sym->type && func_sym->type->return_type) {
                return func_sym->type->return_type;
            }
            return type_create_primitive(TYPE_F64);
        }
        
        default:
            return type_create_primitive(TYPE_UNKNOWN);
    }
}

static void semantic_check_stmt(SemanticAnalyzer *sa, ASTNode *stmt) {
    if (!stmt) return;
    
    switch (stmt->type) {
        case AST_VAR_DECL: {
            ASTVarDecl *var = (ASTVarDecl *)stmt->data;
            WhiteType *var_type = type_create_primitive(TYPE_F64);
            
            if (var->type_node) {
                ASTIdent *type_ident = (ASTIdent *)var->type_node->data;
                /* Look up type */
                Symbol *type_sym = symbol_table_lookup(sa->current_scope, type_ident->name);
                if (type_sym && type_sym->kind == SYMBOL_TYPE_ALIAS) {
                    var_type = type_sym->type;
                }
            }
            
            if (var->init_value) {
                WhiteType *init_type = semantic_check_expr(sa, var->init_value);
                if (!semantic_check_type_compat(var_type, init_type)) {
                    semantic_error(sa, stmt->loc, "type mismatch in variable initialization");
                }
            }
            
            symbol_table_define(sa->current_scope, var->name, var_type, SYMBOL_VAR_LET);
            break;
        }
        
        case AST_IF_STMT: {
            ASTIfStmt *ifstmt = (ASTIfStmt *)stmt->data;
            WhiteType *cond_type = semantic_check_expr(sa, ifstmt->condition);
            if (cond_type && cond_type->kind != TYPE_BOOL) {
                semantic_error(sa, stmt->loc, "if condition must be boolean");
            }
            semantic_check_stmt(sa, ifstmt->then_branch);
            if (ifstmt->else_branch) {
                semantic_check_stmt(sa, ifstmt->else_branch);
            }
            break;
        }
        
        case AST_WHILE_STMT: {
            ASTWhileStmt *whilestmt = (ASTWhileStmt *)stmt->data;
            WhiteType *cond_type = semantic_check_expr(sa, whilestmt->condition);
            if (cond_type && cond_type->kind != TYPE_BOOL) {
                semantic_error(sa, stmt->loc, "while condition must be boolean");
            }
            semantic_check_stmt(sa, whilestmt->body);
            break;
        }
        
        case AST_FOR_STMT: {
            ASTForStmt *forstmt = (ASTForStmt *)stmt->data;
            semantic_enter_scope(sa);
            if (forstmt->init) semantic_check_stmt(sa, forstmt->init);
            if (forstmt->condition) semantic_check_expr(sa, forstmt->condition);
            if (forstmt->update) semantic_check_expr(sa, forstmt->update);
            semantic_check_stmt(sa, forstmt->body);
            semantic_exit_scope(sa);
            break;
        }
        
        case AST_BLOCK: {
            semantic_enter_scope(sa);
            ASTBlock *block = (ASTBlock *)stmt->data;
            for (size_t i = 0; i < block->statements->count; i++) {
                ASTNode *s = (ASTNode *)array_get(block->statements, i);
                semantic_check_stmt(sa, s);
            }
            semantic_exit_scope(sa);
            break;
        }
        
        default:
            break;
    }
}

void semantic_analyze(SemanticAnalyzer *sa, ASTNode *program) {
    if (!program || program->type != AST_PROGRAM) return;
    
    semantic_define_builtin_functions(sa);
    
    ASTProgram *prog = (ASTProgram *)program->data;
    for (size_t i = 0; i < prog->statements->count; i++) {
        ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
        semantic_check_stmt(sa, stmt);
    }
}

void semantic_free(SemanticAnalyzer *sa) {
    if (!sa) return;
    
    symbol_table_free(sa->global_scope);
    
    for (size_t i = 0; i < sa->errors->count; i++) {
        xfree(array_get(sa->errors, i));
    }
    array_free(sa->errors);
    
    for (size_t i = 0; i < sa->warnings->count; i++) {
        xfree(array_get(sa->warnings, i));
    }
    array_free(sa->warnings);
    
    xfree(sa);
}
