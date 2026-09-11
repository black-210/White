#ifndef WHITE_LEXER_H
#define WHITE_LEXER_H

#include "common.h"

typedef enum {
    TOK_EOF,
    TOK_ERROR,
    /* Literals */
    TOK_INT,
    TOK_FLOAT,
    TOK_STRING,
    TOK_TRUE,
    TOK_FALSE,
    /* Keywords */
    TOK_FN,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_FOR,
    TOK_BREAK,
    TOK_CONTINUE,
    TOK_RETURN,
    TOK_STRUCT,
    TOK_ENUM,
    TOK_CONST,
    TOK_USE,
    TOK_PRINT,
    /* Identifiers and types */
    TOK_IDENT,
    TOK_TYPE,
    /* Operators */
    TOK_PLUS,
    TOK_MINUS,
    TOK_STAR,
    TOK_SLASH,
    TOK_PERCENT,
    TOK_EQ,
    TOK_NE,
    TOK_LT,
    TOK_LE,
    TOK_GT,
    TOK_GE,
    TOK_AND,
    TOK_OR,
    TOK_NOT,
    TOK_ASSIGN,
    TOK_PLUS_ASSIGN,
    TOK_MINUS_ASSIGN,
    TOK_STAR_ASSIGN,
    TOK_SLASH_ASSIGN,
    TOK_CARET,
    TOK_AMP,
    TOK_PIPE,
    TOK_LSHIFT,
    TOK_RSHIFT,
    /* Punctuation */
    TOK_LPAREN,
    TOK_RPAREN,
    TOK_LBRACE,
    TOK_RBRACE,
    TOK_LBRACKET,
    TOK_RBRACKET,
    TOK_SEMICOLON,
    TOK_COMMA,
    TOK_DOT,
    TOK_ARROW,
    TOK_COLON,
    TOK_DOUBLE_COLON,
    /* Units */
    TOK_HZ,
    TOK_KHZ,
    TOK_MHZ,
    TOK_GHZ,
    TOK_MS,
    TOK_US,
    TOK_NS,
} TokenType;

typedef struct {
    TokenType type;
    const char *lexeme;
    size_t length;
    SourceLocation loc;
    /* For literals */
    int64_t int_value;
    double float_value;
} Token;

typedef struct {
    const char *source;
    size_t position;
    int line;
    int column;
    const char *filename;
    DynamicArray *tokens;
} Lexer;

Lexer *lexer_create(const char *source, const char *filename);
void lexer_scan_tokens(Lexer *lexer);
Token *lexer_get_tokens(Lexer *lexer, size_t *count);
void lexer_free(Lexer *lexer);

#endif
