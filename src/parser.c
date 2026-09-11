#include "../include/white/parser.h"
#include <string.h>

static bool parser_is_at_end(Parser *p) {
    return p->position >= p->count || p->tokens[p->position].type == TOK_EOF;
}

static Token parser_peek(Parser *p) {
    if (p->position < p->count) return p->tokens[p->position];
    return p->tokens[p->count - 1];
}

static Token parser_previous(Parser *p) {
    if (p->position > 0) return p->tokens[p->position - 1];
    return p->tokens[0];
}

static Token parser_advance(Parser *p) {
    if (!parser_is_at_end(p)) p->position++;
    return parser_previous(p);
}

static bool parser_check(Parser *p, TokenType type) {
    if (parser_is_at_end(p)) return false;
    return parser_peek(p).type == type;
}

static bool parser_match(Parser *p, TokenType type) {
    if (parser_check(p, type)) {
        parser_advance(p);
        return true;
    }
    return false;
}

static ASTNode *parser_parse_expression(Parser *p);
static ASTNode *parser_parse_statement(Parser *p);
static ASTNode *parser_parse_block(Parser *p);

static ASTNode *parser_parse_primary(Parser *p) {
    SourceLocation loc = parser_peek(p).loc;
    
    if (parser_match(p, TOK_TRUE)) {
        ASTNode *node = ast_create_node(AST_BOOL_LITERAL, loc);
        ASTBoolLiteral *lit = xmalloc(sizeof(ASTBoolLiteral));
        lit->value = true;
        node->data = lit;
        return node;
    }
    
    if (parser_match(p, TOK_FALSE)) {
        ASTNode *node = ast_create_node(AST_BOOL_LITERAL, loc);
        ASTBoolLiteral *lit = xmalloc(sizeof(ASTBoolLiteral));
        lit->value = false;
        node->data = lit;
        return node;
    }
    
    if (parser_match(p, TOK_INT)) {
        Token tok = parser_previous(p);
        return ast_create_int_literal(tok.int_value, loc);
    }
    
    if (parser_match(p, TOK_FLOAT)) {
        Token tok = parser_previous(p);
        return ast_create_float_literal(tok.float_value, loc);
    }
    
    if (parser_match(p, TOK_STRING)) {
        Token tok = parser_previous(p);
        char *str = xmalloc(tok.length + 1);
        strncpy(str, tok.lexeme, tok.length);
        str[tok.length] = 0;
        ASTNode *node = ast_create_string_literal(str, loc);
        xfree(str);
        return node;
    }
    
    if (parser_match(p, TOK_IDENT)) {
        Token tok = parser_previous(p);
        char *name = xmalloc(tok.length + 1);
        strncpy(name, tok.lexeme, tok.length);
        name[tok.length] = 0;
        ASTNode *node = ast_create_ident(name, loc);
        xfree(name);
        return node;
    }
    
    if (parser_match(p, TOK_LPAREN)) {
        ASTNode *expr = parser_parse_expression(p);
        parser_match(p, TOK_RPAREN);
        return expr;
    }
    
    return NULL;
}

static ASTNode *parser_parse_postfix(Parser *p) {
    ASTNode *expr = parser_parse_primary(p);
    
    while (true) {
        SourceLocation loc = parser_peek(p).loc;
        
        if (parser_match(p, TOK_LPAREN)) {
            DynamicArray *args = array_create();
            if (!parser_check(p, TOK_RPAREN)) {
                do {
                    ASTNode *arg = parser_parse_expression(p);
                    array_push(args, arg);
                } while (parser_match(p, TOK_COMMA));
            }
            parser_match(p, TOK_RPAREN);
            expr = ast_create_call(expr, args, loc);
        } else if (parser_match(p, TOK_LBRACKET)) {
            ASTNode *index = parser_parse_expression(p);
            parser_match(p, TOK_RBRACKET);
            ASTNode *node = ast_create_node(AST_INDEX, loc);
            ASTIndex *idx = xmalloc(sizeof(ASTIndex));
            idx->array = expr;
            idx->index = index;
            node->data = idx;
            expr = node;
        } else if (parser_match(p, TOK_DOT)) {
            if (parser_match(p, TOK_IDENT)) {
                Token tok = parser_previous(p);
                char *field = xmalloc(tok.length + 1);
                strncpy(field, tok.lexeme, tok.length);
                field[tok.length] = 0;
                ASTNode *node = ast_create_node(AST_MEMBER, loc);
                ASTMember *mem = xmalloc(sizeof(ASTMember));
                mem->object = expr;
                mem->field = field;
                node->data = mem;
                expr = node;
                xfree(field);
            }
        } else {
            break;
        }
    }
    
    return expr;
}

static ASTNode *parser_parse_unary(Parser *p) {
    SourceLocation loc = parser_peek(p).loc;
    
    if (parser_match(p, TOK_NOT)) {
        ASTNode *expr = parser_parse_unary(p);
        return ast_create_unary_op(TOK_NOT, expr, loc);
    }
    if (parser_match(p, TOK_MINUS)) {
        ASTNode *expr = parser_parse_unary(p);
        return ast_create_unary_op(TOK_MINUS, expr, loc);
    }
    if (parser_match(p, TOK_PLUS)) {
        ASTNode *expr = parser_parse_unary(p);
        return ast_create_unary_op(TOK_PLUS, expr, loc);
    }
    if (parser_match(p, TOK_AMP)) {
        ASTNode *expr = parser_parse_unary(p);
        return ast_create_unary_op(TOK_AMP, expr, loc);
    }
    if (parser_match(p, TOK_STAR)) {
        ASTNode *expr = parser_parse_unary(p);
        return ast_create_unary_op(TOK_STAR, expr, loc);
    }
    
    return parser_parse_postfix(p);
}

static ASTNode *parser_parse_multiplicative(Parser *p) {
    ASTNode *expr = parser_parse_unary(p);
    
    while (parser_match(p, TOK_STAR) || parser_match(p, TOK_SLASH) || parser_match(p, TOK_PERCENT)) {
        TokenType op = parser_previous(p).type;
        SourceLocation loc = parser_previous(p).loc;
        ASTNode *right = parser_parse_unary(p);
        expr = ast_create_binary_op(expr, op, right, loc);
    }
    
    return expr;
}

static ASTNode *parser_parse_additive(Parser *p) {
    ASTNode *expr = parser_parse_multiplicative(p);
    
    while (parser_match(p, TOK_PLUS) || parser_match(p, TOK_MINUS)) {
        TokenType op = parser_previous(p).type;
        SourceLocation loc = parser_previous(p).loc;
        ASTNode *right = parser_parse_multiplicative(p);
        expr = ast_create_binary_op(expr, op, right, loc);
    }
    
    return expr;
}

static ASTNode *parser_parse_shift(Parser *p) {
    ASTNode *expr = parser_parse_additive(p);
    
    while (parser_match(p, TOK_LSHIFT) || parser_match(p, TOK_RSHIFT)) {
        TokenType op = parser_previous(p).type;
        SourceLocation loc = parser_previous(p).loc;
        ASTNode *right = parser_parse_additive(p);
        expr = ast_create_binary_op(expr, op, right, loc);
    }
    
    return expr;
}

static ASTNode *parser_parse_comparison(Parser *p) {
    ASTNode *expr = parser_parse_shift(p);
    
    while (parser_match(p, TOK_LT) || parser_match(p, TOK_LE) ||
           parser_match(p, TOK_GT) || parser_match(p, TOK_GE)) {
        TokenType op = parser_previous(p).type;
        SourceLocation loc = parser_previous(p).loc;
        ASTNode *right = parser_parse_shift(p);
        expr = ast_create_binary_op(expr, op, right, loc);
    }
    
    return expr;
}

static ASTNode *parser_parse_equality(Parser *p) {
    ASTNode *expr = parser_parse_comparison(p);
    
    while (parser_match(p, TOK_EQ) || parser_match(p, TOK_NE)) {
        TokenType op = parser_previous(p).type;
        SourceLocation loc = parser_previous(p).loc;
        ASTNode *right = parser_parse_comparison(p);
        expr = ast_create_binary_op(expr, op, right, loc);
    }
    
    return expr;
}

static ASTNode *parser_parse_logical_and(Parser *p) {
    ASTNode *expr = parser_parse_equality(p);
    
    while (parser_match(p, TOK_AND)) {
        SourceLocation loc = parser_previous(p).loc;
        ASTNode *right = parser_parse_equality(p);
        expr = ast_create_binary_op(expr, TOK_AND, right, loc);
    }
    
    return expr;
}

static ASTNode *parser_parse_logical_or(Parser *p) {
    ASTNode *expr = parser_parse_logical_and(p);
    
    while (parser_match(p, TOK_OR)) {
        SourceLocation loc = parser_previous(p).loc;
        ASTNode *right = parser_parse_logical_and(p);
        expr = ast_create_binary_op(expr, TOK_OR, right, loc);
    }
    
    return expr;
}

static ASTNode *parser_parse_assignment(Parser *p) {
    ASTNode *expr = parser_parse_logical_or(p);
    
    if (parser_match(p, TOK_ASSIGN) || parser_match(p, TOK_PLUS_ASSIGN) ||
        parser_match(p, TOK_MINUS_ASSIGN) || parser_match(p, TOK_STAR_ASSIGN) ||
        parser_match(p, TOK_SLASH_ASSIGN)) {
        TokenType op = parser_previous(p).type;
        SourceLocation loc = parser_previous(p).loc;
        ASTNode *value = parser_parse_assignment(p);
        expr = ast_create_binary_op(expr, op, value, loc);
    }
    
    return expr;
}

static ASTNode *parser_parse_expression(Parser *p) {
    return parser_parse_assignment(p);
}

static ASTNode *parser_parse_var_decl(Parser *p) {
    SourceLocation loc = parser_peek(p).loc;
    Token name_tok = parser_peek(p);
    parser_match(p, TOK_IDENT);
    
    char *name = xmalloc(name_tok.length + 1);
    strncpy(name, name_tok.lexeme, name_tok.length);
    name[name_tok.length] = 0;
    
    ASTNode *type = NULL;
    if (parser_match(p, TOK_COLON)) {
        if (parser_match(p, TOK_IDENT)) {
            Token type_tok = parser_previous(p);
            char *type_name = xmalloc(type_tok.length + 1);
            strncpy(type_name, type_tok.lexeme, type_tok.length);
            type_name[type_tok.length] = 0;
            type = ast_create_ident(type_name, type_tok.loc);
            xfree(type_name);
        }
    }
    
    ASTNode *init = NULL;
    if (parser_match(p, TOK_ASSIGN)) {
        init = parser_parse_expression(p);
    }
    
    ASTNode *decl = ast_create_var_decl(name, type, init, loc);
    xfree(name);
    return decl;
}

static ASTNode *parser_parse_return_stmt(Parser *p) {
    SourceLocation loc = parser_previous(p).loc;
    ASTNode *value = NULL;
    if (!parser_check(p, TOK_SEMICOLON) && !parser_is_at_end(p)) {
        value = parser_parse_expression(p);
    }
    parser_match(p, TOK_SEMICOLON);
    return ast_create_return_stmt(value, loc);
}

static ASTNode *parser_parse_if_stmt(Parser *p) {
    SourceLocation loc = parser_previous(p).loc;
    parser_match(p, TOK_LPAREN);
    ASTNode *cond = parser_parse_expression(p);
    parser_match(p, TOK_RPAREN);
    
    ASTNode *then_branch = parser_parse_statement(p);
    ASTNode *else_branch = NULL;
    if (parser_match(p, TOK_ELSE)) {
        else_branch = parser_parse_statement(p);
    }
    
    return ast_create_if_stmt(cond, then_branch, else_branch, loc);
}

static ASTNode *parser_parse_while_stmt(Parser *p) {
    SourceLocation loc = parser_previous(p).loc;
    parser_match(p, TOK_LPAREN);
    ASTNode *cond = parser_parse_expression(p);
    parser_match(p, TOK_RPAREN);
    ASTNode *body = parser_parse_statement(p);
    return ast_create_while_stmt(cond, body, loc);
}

static ASTNode *parser_parse_for_stmt(Parser *p) {
    SourceLocation loc = parser_previous(p).loc;
    parser_match(p, TOK_LPAREN);
    
    ASTNode *init = NULL;
    if (!parser_check(p, TOK_SEMICOLON)) {
        init = parser_parse_var_decl(p);
    }
    parser_match(p, TOK_SEMICOLON);
    
    ASTNode *cond = NULL;
    if (!parser_check(p, TOK_SEMICOLON)) {
        cond = parser_parse_expression(p);
    }
    parser_match(p, TOK_SEMICOLON);
    
    ASTNode *update = NULL;
    if (!parser_check(p, TOK_RPAREN)) {
        update = parser_parse_expression(p);
    }
    parser_match(p, TOK_RPAREN);
    
    ASTNode *body = parser_parse_statement(p);
    
    ASTNode *node = ast_create_node(AST_FOR_STMT, loc);
    ASTForStmt *forstmt = xmalloc(sizeof(ASTForStmt));
    forstmt->init = init;
    forstmt->condition = cond;
    forstmt->update = update;
    forstmt->body = body;
    node->data = forstmt;
    return node;
}

static ASTNode *parser_parse_print_stmt(Parser *p) {
    SourceLocation loc = parser_previous(p).loc;
    parser_match(p, TOK_LPAREN);
    
    DynamicArray *args = array_create();
    if (!parser_check(p, TOK_RPAREN)) {
        do {
            ASTNode *arg = parser_parse_expression(p);
            array_push(args, arg);
        } while (parser_match(p, TOK_COMMA));
    }
    parser_match(p, TOK_RPAREN);
    parser_match(p, TOK_SEMICOLON);
    
    return ast_create_print_stmt(args, loc);
}

static ASTNode *parser_parse_statement(Parser *p) {
    if (parser_match(p, TOK_IF)) {
        return parser_parse_if_stmt(p);
    }
    if (parser_match(p, TOK_WHILE)) {
        return parser_parse_while_stmt(p);
    }
    if (parser_match(p, TOK_FOR)) {
        return parser_parse_for_stmt(p);
    }
    if (parser_match(p, TOK_RETURN)) {
        return parser_parse_return_stmt(p);
    }
    if (parser_match(p, TOK_BREAK)) {
        SourceLocation loc = parser_previous(p).loc;
        parser_match(p, TOK_SEMICOLON);
        return ast_create_node(AST_BREAK_STMT, loc);
    }
    if (parser_match(p, TOK_CONTINUE)) {
        SourceLocation loc = parser_previous(p).loc;
        parser_match(p, TOK_SEMICOLON);
        return ast_create_node(AST_CONTINUE_STMT, loc);
    }
    if (parser_match(p, TOK_PRINT)) {
        return parser_parse_print_stmt(p);
    }
    if (parser_match(p, TOK_LBRACE)) {
        return parser_parse_block(p);
    }
    
    /* Could be var decl or expression stmt */
    ASTNode *expr = parser_parse_expression(p);
    parser_match(p, TOK_SEMICOLON);
    return expr;
}

static ASTNode *parser_parse_block(Parser *p) {
    ASTNode *block = ast_create_block();
    
    while (!parser_check(p, TOK_RBRACE) && !parser_is_at_end(p)) {
        ASTNode *stmt = parser_parse_statement(p);
        if (stmt) ast_block_add_stmt(block, stmt);
    }
    
    parser_match(p, TOK_RBRACE);
    return block;
}

static ASTNode *parser_parse_function(Parser *p) {
    SourceLocation loc = parser_previous(p).loc;
    
    Token name_tok = parser_peek(p);
    parser_match(p, TOK_IDENT);
    char *name = xmalloc(name_tok.length + 1);
    strncpy(name, name_tok.lexeme, name_tok.length);
    name[name_tok.length] = 0;
    
    parser_match(p, TOK_LPAREN);
    DynamicArray *params = array_create();
    if (!parser_check(p, TOK_RPAREN)) {
        do {
            if (parser_match(p, TOK_IDENT)) {
                Token param_tok = parser_previous(p);
                char *param_name = xmalloc(param_tok.length + 1);
                strncpy(param_name, param_tok.lexeme, param_tok.length);
                param_name[param_tok.length] = 0;
                array_push(params, ast_create_ident(param_name, param_tok.loc));
                xfree(param_name);
            }
        } while (parser_match(p, TOK_COMMA));
    }
    parser_match(p, TOK_RPAREN);
    
    ASTNode *return_type = NULL;
    if (parser_match(p, TOK_ARROW)) {
        if (parser_match(p, TOK_IDENT)) {
            Token type_tok = parser_previous(p);
            char *type_name = xmalloc(type_tok.length + 1);
            strncpy(type_name, type_tok.lexeme, type_tok.length);
            type_name[type_tok.length] = 0;
            return_type = ast_create_ident(type_name, type_tok.loc);
            xfree(type_name);
        }
    }
    
    parser_match(p, TOK_LBRACE);
    ASTNode *body = parser_parse_block(p);
    
    ASTNode *func = ast_create_func_def(name, params, return_type, body, loc);
    xfree(name);
    return func;
}

Parser *parser_create(Token *tokens, size_t count, const char *filename) {
    Parser *p = xmalloc(sizeof(Parser));
    p->tokens = tokens;
    p->count = count;
    p->position = 0;
    p->filename = filename;
    p->errors = array_create();
    return p;
}

ASTNode *parser_parse(Parser *p) {
    ASTNode *program = ast_create_program();
    
    while (!parser_is_at_end(p)) {
        if (parser_match(p, TOK_FN)) {
            ASTNode *func = parser_parse_function(p);
            if (func) ast_program_add_stmt(program, func);
        } else if (parser_match(p, TOK_CONST)) {
            ASTNode *var = parser_parse_var_decl(p);
            parser_match(p, TOK_SEMICOLON);
            if (var) ast_program_add_stmt(program, var);
        } else {
            ASTNode *stmt = parser_parse_statement(p);
            if (stmt) ast_program_add_stmt(program, stmt);
        }
    }
    
    return program;
}

void parser_free(Parser *p) {
    array_free(p->errors);
    xfree(p);
}
