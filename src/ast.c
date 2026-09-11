#include "../include/white/ast.h"
#include <string.h>

static ASTNode *ast_create_node(ASTNodeType type, SourceLocation loc) {
    ASTNode *node = xmalloc(sizeof(ASTNode));
    node->type = type;
    node->loc = loc;
    node->data = NULL;
    return node;
}

ASTNode *ast_create_program(void) {
    ASTNode *node = ast_create_node(AST_PROGRAM, (SourceLocation){NULL, 0, 0});
    ASTProgram *prog = xmalloc(sizeof(ASTProgram));
    prog->statements = array_create();
    node->data = prog;
    return node;
}

ASTNode *ast_create_int_literal(int64_t value, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_INT_LITERAL, loc);
    ASTIntLiteral *lit = xmalloc(sizeof(ASTIntLiteral));
    lit->value = value;
    node->data = lit;
    return node;
}

ASTNode *ast_create_float_literal(double value, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_FLOAT_LITERAL, loc);
    ASTFloatLiteral *lit = xmalloc(sizeof(ASTFloatLiteral));
    lit->value = value;
    node->data = lit;
    return node;
}

ASTNode *ast_create_string_literal(const char *value, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_STRING_LITERAL, loc);
    ASTStringLiteral *lit = xmalloc(sizeof(ASTStringLiteral));
    lit->value = str_dup(value);
    node->data = lit;
    return node;
}

ASTNode *ast_create_ident(const char *name, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_IDENT, loc);
    ASTIdent *ident = xmalloc(sizeof(ASTIdent));
    ident->name = str_dup(name);
    node->data = ident;
    return node;
}

ASTNode *ast_create_binary_op(ASTNode *left, TokenType op, ASTNode *right, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_BINARY_OP, loc);
    ASTBinaryOp *binop = xmalloc(sizeof(ASTBinaryOp));
    binop->left = left;
    binop->op = op;
    binop->right = right;
    node->data = binop;
    return node;
}

ASTNode *ast_create_unary_op(TokenType op, ASTNode *operand, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_UNARY_OP, loc);
    ASTUnaryOp *unop = xmalloc(sizeof(ASTUnaryOp));
    unop->op = op;
    unop->operand = operand;
    node->data = unop;
    return node;
}

ASTNode *ast_create_call(ASTNode *func, DynamicArray *args, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_CALL, loc);
    ASTCall *call = xmalloc(sizeof(ASTCall));
    call->func = func;
    call->args = args ? args : array_create();
    node->data = call;
    return node;
}

ASTNode *ast_create_block(void) {
    ASTNode *node = ast_create_node(AST_BLOCK, (SourceLocation){NULL, 0, 0});
    ASTBlock *block = xmalloc(sizeof(ASTBlock));
    block->statements = array_create();
    node->data = block;
    return node;
}

ASTNode *ast_create_if_stmt(ASTNode *cond, ASTNode *then_br, ASTNode *else_br, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_IF_STMT, loc);
    ASTIfStmt *ifstmt = xmalloc(sizeof(ASTIfStmt));
    ifstmt->condition = cond;
    ifstmt->then_branch = then_br;
    ifstmt->else_branch = else_br;
    node->data = ifstmt;
    return node;
}

ASTNode *ast_create_while_stmt(ASTNode *cond, ASTNode *body, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_WHILE_STMT, loc);
    ASTWhileStmt *whilestmt = xmalloc(sizeof(ASTWhileStmt));
    whilestmt->condition = cond;
    whilestmt->body = body;
    node->data = whilestmt;
    return node;
}

ASTNode *ast_create_return_stmt(ASTNode *value, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_RETURN_STMT, loc);
    ASTReturnStmt *ret = xmalloc(sizeof(ASTReturnStmt));
    ret->value = value;
    node->data = ret;
    return node;
}

ASTNode *ast_create_func_def(const char *name, DynamicArray *params, ASTNode *return_type, ASTNode *body, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_FUNC_DEF, loc);
    ASTFuncDef *func = xmalloc(sizeof(ASTFuncDef));
    func->name = str_dup(name);
    func->params = params ? params : array_create();
    func->return_type = return_type;
    func->body = body;
    node->data = func;
    return node;
}

ASTNode *ast_create_var_decl(const char *name, ASTNode *type, ASTNode *init, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_VAR_DECL, loc);
    ASTVarDecl *var = xmalloc(sizeof(ASTVarDecl));
    var->name = str_dup(name);
    var->type = type;
    var->init_value = init;
    node->data = var;
    return node;
}

ASTNode *ast_create_print_stmt(DynamicArray *args, SourceLocation loc) {
    ASTNode *node = ast_create_node(AST_PRINT_STMT, loc);
    node->data = args ? args : array_create();
    return node;
}

void ast_block_add_stmt(ASTNode *block, ASTNode *stmt) {
    if (block && block->type == AST_BLOCK) {
        ASTBlock *b = (ASTBlock *)block->data;
        array_push(b->statements, stmt);
    }
}

void ast_program_add_stmt(ASTNode *program, ASTNode *stmt) {
    if (program && program->type == AST_PROGRAM) {
        ASTProgram *p = (ASTProgram *)program->data;
        array_push(p->statements, stmt);
    }
}

void ast_call_add_arg(ASTNode *call, ASTNode *arg) {
    if (call && call->type == AST_CALL) {
        ASTCall *c = (ASTCall *)call->data;
        array_push(c->args, arg);
    }
}

void ast_func_add_param(ASTNode *func, ASTNode *param) {
    if (func && func->type == AST_FUNC_DEF) {
        ASTFuncDef *f = (ASTFuncDef *)func->data;
        array_push(f->params, param);
    }
}

static void ast_free_node(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_PROGRAM: {
            ASTProgram *prog = (ASTProgram *)node->data;
            for (size_t i = 0; i < prog->statements->count; i++) {
                ast_free_node((ASTNode *)array_get(prog->statements, i));
            }
            array_free(prog->statements);
            xfree(prog);
            break;
        }
        case AST_BLOCK: {
            ASTBlock *block = (ASTBlock *)node->data;
            for (size_t i = 0; i < block->statements->count; i++) {
                ast_free_node((ASTNode *)array_get(block->statements, i));
            }
            array_free(block->statements);
            xfree(block);
            break;
        }
        case AST_FUNC_DEF: {
            ASTFuncDef *func = (ASTFuncDef *)node->data;
            xfree(func->name);
            if (func->return_type) ast_free_node(func->return_type);
            if (func->body) ast_free_node(func->body);
            array_free(func->params);
            xfree(func);
            break;
        }
        case AST_VAR_DECL: {
            ASTVarDecl *var = (ASTVarDecl *)node->data;
            xfree(var->name);
            if (var->type) ast_free_node(var->type);
            if (var->init_value) ast_free_node(var->init_value);
            xfree(var);
            break;
        }
        case AST_STRING_LITERAL: {
            ASTStringLiteral *str = (ASTStringLiteral *)node->data;
            xfree(str->value);
            xfree(str);
            break;
        }
        case AST_IDENT: {
            ASTIdent *ident = (ASTIdent *)node->data;
            xfree(ident->name);
            xfree(ident);
            break;
        }
        case AST_BINARY_OP: {
            ASTBinaryOp *binop = (ASTBinaryOp *)node->data;
            ast_free_node(binop->left);
            ast_free_node(binop->right);
            xfree(binop);
            break;
        }
        case AST_UNARY_OP: {
            ASTUnaryOp *unop = (ASTUnaryOp *)node->data;
            ast_free_node(unop->operand);
            xfree(unop);
            break;
        }
        case AST_CALL: {
            ASTCall *call = (ASTCall *)node->data;
            ast_free_node(call->func);
            for (size_t i = 0; i < call->args->count; i++) {
                ast_free_node((ASTNode *)array_get(call->args, i));
            }
            array_free(call->args);
            xfree(call);
            break;
        }
        case AST_IF_STMT: {
            ASTIfStmt *ifstmt = (ASTIfStmt *)node->data;
            ast_free_node(ifstmt->condition);
            ast_free_node(ifstmt->then_branch);
            if (ifstmt->else_branch) ast_free_node(ifstmt->else_branch);
            xfree(ifstmt);
            break;
        }
        case AST_WHILE_STMT: {
            ASTWhileStmt *whilestmt = (ASTWhileStmt *)node->data;
            ast_free_node(whilestmt->condition);
            ast_free_node(whilestmt->body);
            xfree(whilestmt);
            break;
        }
        case AST_RETURN_STMT: {
            ASTReturnStmt *ret = (ASTReturnStmt *)node->data;
            if (ret->value) ast_free_node(ret->value);
            xfree(ret);
            break;
        }
        case AST_PRINT_STMT: {
            DynamicArray *args = (DynamicArray *)node->data;
            for (size_t i = 0; i < args->count; i++) {
                ast_free_node((ASTNode *)array_get(args, i));
            }
            array_free(args);
            break;
        }
        default:
            if (node->data) xfree(node->data);
            break;
    }
    xfree(node);
}

void ast_free(ASTNode *node) {
    ast_free_node(node);
}
