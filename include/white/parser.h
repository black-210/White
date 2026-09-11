#ifndef WHITE_PARSER_H
#define WHITE_PARSER_H

#include "common.h"
#include "lexer.h"
#include "ast.h"

typedef struct {
    Token *tokens;
    size_t count;
    size_t position;
    const char *filename;
    DynamicArray *errors;
} Parser;

Parser *parser_create(Token *tokens, size_t count, const char *filename);
ASTNode *parser_parse(Parser *parser);
void parser_free(Parser *parser);

#endif
