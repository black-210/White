#ifndef WHITE_AST_H
#define WHITE_AST_H

#include "common.h"
#include "lexer.h"

typedef enum {
    AST_PROGRAM,
    AST_FUNC_DEF,
    AST_VAR_DECL,
    AST_CONST_DECL,
    AST_STRUCT_DEF,
    AST_ENUM_DEF,
    AST_TYPE_ALIAS,
    AST_BLOCK,
    AST_EXPR_STMT,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_FOR_STMT,
    AST_BREAK_STMT,
    AST_CONTINUE_STMT,
    AST_RETURN_STMT,
    AST_PRINT_STMT,
    /* Expressions */
    AST_INT_LITERAL,
    AST_FLOAT_LITERAL,
    AST_STRING_LITERAL,
    AST_BOOL_LITERAL,
    AST_IDENT,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_CALL,
    AST_INDEX,
    AST_MEMBER,
    AST_ARRAY_LITERAL,
    AST_STRUCT_LITERAL,
    AST_CAST,
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    SourceLocation loc;
    void *data;
} ASTNode;

typedef struct {
    DynamicArray *statements;
} ASTProgram;

typedef struct {
    char *name;
    DynamicArray *params;
    ASTNode *return_type;
    ASTNode *body;
} ASTFuncDef;

typedef struct {
    char *name;
    ASTNode *type;
    ASTNode *init_value;
} ASTVarDecl;

typedef struct {
    int64_t value;
} ASTIntLiteral;

typedef struct {
    double value;
} ASTFloatLiteral;

typedef struct {
    char *value;
} ASTStringLiteral;

typedef struct {
    bool value;
} ASTBoolLiteral;

typedef struct {
    char *name;
} ASTIdent;

typedef struct {
    ASTNode *left;
    TokenType op;
    ASTNode *right;
} ASTBinaryOp;

typedef struct {
    TokenType op;
    ASTNode *operand;
} ASTUnaryOp;

typedef struct {
    ASTNode *func;
    DynamicArray *args;
} ASTCall;

typedef struct {
    ASTNode *array;
    ASTNode *index;
} ASTIndex;

typedef struct {
    ASTNode *object;
    char *field;
} ASTMember;

typedef struct {
    DynamicArray *statements;
} ASTBlock;

typedef struct {
    ASTNode *condition;
    ASTNode *then_branch;
    ASTNode *else_branch;
} ASTIfStmt;

typedef struct {
    ASTNode *condition;
    ASTNode *body;
} ASTWhileStmt;

typedef struct {
    ASTNode *init;
    ASTNode *condition;
    ASTNode *update;
    ASTNode *body;
} ASTForStmt;

typedef struct {
    ASTNode *value;
} ASTReturnStmt;

/* Node creation */
ASTNode *ast_create_program(void);
ASTNode *ast_create_int_literal(int64_t value, SourceLocation loc);
ASTNode *ast_create_float_literal(double value, SourceLocation loc);
ASTNode *ast_create_string_literal(const char *value, SourceLocation loc);
ASTNode *ast_create_ident(const char *name, SourceLocation loc);
ASTNode *ast_create_binary_op(ASTNode *left, TokenType op, ASTNode *right, SourceLocation loc);
ASTNode *ast_create_unary_op(TokenType op, ASTNode *operand, SourceLocation loc);
ASTNode *ast_create_call(ASTNode *func, DynamicArray *args, SourceLocation loc);
ASTNode *ast_create_block(void);
ASTNode *ast_create_if_stmt(ASTNode *cond, ASTNode *then_br, ASTNode *else_br, SourceLocation loc);
ASTNode *ast_create_while_stmt(ASTNode *cond, ASTNode *body, SourceLocation loc);
ASTNode *ast_create_return_stmt(ASTNode *value, SourceLocation loc);
ASTNode *ast_create_func_def(const char *name, DynamicArray *params, ASTNode *return_type, ASTNode *body, SourceLocation loc);
ASTNode *ast_create_var_decl(const char *name, ASTNode *type, ASTNode *init, SourceLocation loc);
ASTNode *ast_create_print_stmt(DynamicArray *args, SourceLocation loc);

void ast_block_add_stmt(ASTNode *block, ASTNode *stmt);
void ast_program_add_stmt(ASTNode *program, ASTNode *stmt);
void ast_call_add_arg(ASTNode *call, ASTNode *arg);
void ast_func_add_param(ASTNode *func, ASTNode *param);

void ast_free(ASTNode *node);

#endif
