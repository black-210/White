#include "../include/white/lexer.h"
#include <ctype.h>
#include <string.h>

static bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

static bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

static bool is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

static const char *get_keyword_or_type(const char *lexeme, size_t len) {
    if (len == 2 && strncmp(lexeme, "fn", 2) == 0) return "fn";
    if (len == 2 && strncmp(lexeme, "if", 2) == 0) return "if";
    if (len == 4 && strncmp(lexeme, "else", 4) == 0) return "else";
    if (len == 5 && strncmp(lexeme, "while", 5) == 0) return "while";
    if (len == 3 && strncmp(lexeme, "for", 3) == 0) return "for";
    if (len == 5 && strncmp(lexeme, "break", 5) == 0) return "break";
    if (len == 8 && strncmp(lexeme, "continue", 8) == 0) return "continue";
    if (len == 6 && strncmp(lexeme, "return", 6) == 0) return "return";
    if (len == 6 && strncmp(lexeme, "struct", 6) == 0) return "struct";
    if (len == 4 && strncmp(lexeme, "enum", 4) == 0) return "enum";
    if (len == 5 && strncmp(lexeme, "const", 5) == 0) return "const";
    if (len == 3 && strncmp(lexeme, "use", 3) == 0) return "use";
    if (len == 5 && strncmp(lexeme, "print", 5) == 0) return "print";
    if (len == 4 && strncmp(lexeme, "true", 4) == 0) return "true";
    if (len == 5 && strncmp(lexeme, "false", 5) == 0) return "false";
    return NULL;
}

Lexer *lexer_create(const char *source, const char *filename) {
    Lexer *lexer = xmalloc(sizeof(Lexer));
    lexer->source = source;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->filename = filename;
    lexer->tokens = array_create();
    return lexer;
}

static char lexer_peek(Lexer *lexer, int offset) {
    return lexer->source[lexer->position + offset];
}

static void lexer_advance(Lexer *lexer) {
    if (lexer->source[lexer->position] == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    lexer->position++;
}

static void lexer_skip_whitespace(Lexer *lexer) {
    while (lexer->position < strlen(lexer->source)) {
        char c = lexer->source[lexer->position];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            lexer_advance(lexer);
        } else if (c == '/' && lexer->source[lexer->position + 1] == '/') {
            while (lexer->source[lexer->position] != '\n' && lexer->source[lexer->position]) {
                lexer_advance(lexer);
            }
        } else {
            break;
        }
    }
}

static Token lexer_make_token(Lexer *lexer, TokenType type, const char *lexeme, size_t length) {
    Token tok;
    tok.type = type;
    tok.lexeme = lexeme;
    tok.length = length;
    tok.loc.filename = lexer->filename;
    tok.loc.line = lexer->line;
    tok.loc.column = lexer->column - length;
    tok.int_value = 0;
    tok.float_value = 0.0;
    return tok;
}

static void lexer_scan_number(Lexer *lexer) {
    size_t start = lexer->position;
    int start_line = lexer->line;
    int start_col = lexer->column;
    
    while (is_digit(lexer->source[lexer->position])) {
        lexer_advance(lexer);
    }
    
    TokenType type = TOK_INT;
    if (lexer->source[lexer->position] == '.' && is_digit(lexer->source[lexer->position + 1])) {
        type = TOK_FLOAT;
        lexer_advance(lexer);
        while (is_digit(lexer->source[lexer->position])) {
            lexer_advance(lexer);
        }
    }
    
    if (lexer->source[lexer->position] == 'e' || lexer->source[lexer->position] == 'E') {
        type = TOK_FLOAT;
        lexer_advance(lexer);
        if (lexer->source[lexer->position] == '+' || lexer->source[lexer->position] == '-') {
            lexer_advance(lexer);
        }
        while (is_digit(lexer->source[lexer->position])) {
            lexer_advance(lexer);
        }
    }
    
    size_t length = lexer->position - start;
    const char *lexeme = lexer->source + start;
    Token tok = lexer_make_token(lexer, type, lexeme, length);
    
    if (type == TOK_INT) {
        tok.int_value = strtoll(lexeme, NULL, 10);
    } else {
        tok.float_value = strtod(lexeme, NULL);
    }
    
    array_push(lexer->tokens, memcpy(xmalloc(sizeof(Token)), &tok, sizeof(Token)));
}

static void lexer_scan_string(Lexer *lexer) {
    char quote = lexer->source[lexer->position];
    lexer_advance(lexer);
    size_t start = lexer->position;
    
    while (lexer->source[lexer->position] && lexer->source[lexer->position] != quote) {
        if (lexer->source[lexer->position] == '\\') {
            lexer_advance(lexer);
        }
        lexer_advance(lexer);
    }
    
    size_t length = lexer->position - start;
    const char *lexeme = lexer->source + start;
    Token tok = lexer_make_token(lexer, TOK_STRING, lexeme, length);
    lexer_advance(lexer);
    array_push(lexer->tokens, memcpy(xmalloc(sizeof(Token)), &tok, sizeof(Token)));
}

static void lexer_scan_identifier(Lexer *lexer) {
    size_t start = lexer->position;
    while (is_alnum(lexer->source[lexer->position])) {
        lexer_advance(lexer);
    }
    
    size_t length = lexer->position - start;
    const char *lexeme = lexer->source + start;
    const char *keyword = get_keyword_or_type(lexeme, length);
    
    TokenType type = TOK_IDENT;
    if (keyword) {
        if (strcmp(keyword, "fn") == 0) type = TOK_FN;
        else if (strcmp(keyword, "if") == 0) type = TOK_IF;
        else if (strcmp(keyword, "else") == 0) type = TOK_ELSE;
        else if (strcmp(keyword, "while") == 0) type = TOK_WHILE;
        else if (strcmp(keyword, "for") == 0) type = TOK_FOR;
        else if (strcmp(keyword, "break") == 0) type = TOK_BREAK;
        else if (strcmp(keyword, "continue") == 0) type = TOK_CONTINUE;
        else if (strcmp(keyword, "return") == 0) type = TOK_RETURN;
        else if (strcmp(keyword, "struct") == 0) type = TOK_STRUCT;
        else if (strcmp(keyword, "enum") == 0) type = TOK_ENUM;
        else if (strcmp(keyword, "const") == 0) type = TOK_CONST;
        else if (strcmp(keyword, "use") == 0) type = TOK_USE;
        else if (strcmp(keyword, "print") == 0) type = TOK_PRINT;
        else if (strcmp(keyword, "true") == 0) type = TOK_TRUE;
        else if (strcmp(keyword, "false") == 0) type = TOK_FALSE;
    }
    
    Token tok = lexer_make_token(lexer, type, lexeme, length);
    array_push(lexer->tokens, memcpy(xmalloc(sizeof(Token)), &tok, sizeof(Token)));
}

void lexer_scan_tokens(Lexer *lexer) {
    while (lexer->position < strlen(lexer->source)) {
        lexer_skip_whitespace(lexer);
        if (lexer->position >= strlen(lexer->source)) break;
        
        char c = lexer->source[lexer->position];
        size_t start = lexer->position;
        int col = lexer->column;
        
        if (is_digit(c)) {
            lexer_scan_number(lexer);
        } else if (c == '"' || c == '\'') {
            lexer_scan_string(lexer);
        } else if (is_alpha(c)) {
            lexer_scan_identifier(lexer);
        } else {
            TokenType type = TOK_ERROR;
            if (c == '+') {
                lexer_advance(lexer);
                if (lexer->source[lexer->position] == '=') {
                    type = TOK_PLUS_ASSIGN;
                    lexer_advance(lexer);
                } else {
                    type = TOK_PLUS;
                }
            } else if (c == '-') {
                lexer_advance(lexer);
                if (lexer->source[lexer->position] == '=') {
                    type = TOK_MINUS_ASSIGN;
                    lexer_advance(lexer);
                } else if (lexer->source[lexer->position] == '>') {
                    type = TOK_ARROW;
                    lexer_advance(lexer);
                } else {
                    type = TOK_MINUS;
                }
            } else if (c == '*') {
                lexer_advance(lexer);
                type = lexer->source[lexer->position] == '=' ? TOK_STAR_ASSIGN : TOK_STAR;
                if (lexer->source[lexer->position] == '=') lexer_advance(lexer);
            } else if (c == '/') {
                lexer_advance(lexer);
                type = lexer->source[lexer->position] == '=' ? TOK_SLASH_ASSIGN : TOK_SLASH;
                if (lexer->source[lexer->position] == '=') lexer_advance(lexer);
            } else if (c == '=') {
                lexer_advance(lexer);
                type = lexer->source[lexer->position] == '=' ? TOK_EQ : TOK_ASSIGN;
                if (lexer->source[lexer->position] == '=') lexer_advance(lexer);
            } else if (c == '!') {
                lexer_advance(lexer);
                type = lexer->source[lexer->position] == '=' ? TOK_NE : TOK_NOT;
                if (lexer->source[lexer->position] == '=') lexer_advance(lexer);
            } else if (c == '<') {
                lexer_advance(lexer);
                if (lexer->source[lexer->position] == '=') {
                    type = TOK_LE;
                    lexer_advance(lexer);
                } else if (lexer->source[lexer->position] == '<') {
                    type = TOK_LSHIFT;
                    lexer_advance(lexer);
                } else {
                    type = TOK_LT;
                }
            } else if (c == '>') {
                lexer_advance(lexer);
                if (lexer->source[lexer->position] == '=') {
                    type = TOK_GE;
                    lexer_advance(lexer);
                } else if (lexer->source[lexer->position] == '>') {
                    type = TOK_RSHIFT;
                    lexer_advance(lexer);
                } else {
                    type = TOK_GT;
                }
            } else if (c == '&') {
                lexer_advance(lexer);
                type = lexer->source[lexer->position] == '&' ? TOK_AND : TOK_AMP;
                if (lexer->source[lexer->position] == '&') lexer_advance(lexer);
            } else if (c == '|') {
                lexer_advance(lexer);
                type = lexer->source[lexer->position] == '|' ? TOK_OR : TOK_PIPE;
                if (lexer->source[lexer->position] == '|') lexer_advance(lexer);
            } else if (c == '(') {
                type = TOK_LPAREN;
                lexer_advance(lexer);
            } else if (c == ')') {
                type = TOK_RPAREN;
                lexer_advance(lexer);
            } else if (c == '{') {
                type = TOK_LBRACE;
                lexer_advance(lexer);
            } else if (c == '}') {
                type = TOK_RBRACE;
                lexer_advance(lexer);
            } else if (c == '[') {
                type = TOK_LBRACKET;
                lexer_advance(lexer);
            } else if (c == ']') {
                type = TOK_RBRACKET;
                lexer_advance(lexer);
            } else if (c == ';') {
                type = TOK_SEMICOLON;
                lexer_advance(lexer);
            } else if (c == ',') {
                type = TOK_COMMA;
                lexer_advance(lexer);
            } else if (c == '.') {
                type = TOK_DOT;
                lexer_advance(lexer);
            } else if (c == ':') {
                lexer_advance(lexer);
                type = lexer->source[lexer->position] == ':' ? TOK_DOUBLE_COLON : TOK_COLON;
                if (lexer->source[lexer->position] == ':') lexer_advance(lexer);
            } else if (c == '%') {
                type = TOK_PERCENT;
                lexer_advance(lexer);
            } else if (c == '^') {
                type = TOK_CARET;
                lexer_advance(lexer);
            } else {
                lexer_advance(lexer);
            }
            
            if (type != TOK_ERROR) {
                Token tok = lexer_make_token(lexer, type, lexer->source + start, lexer->position - start);
                array_push(lexer->tokens, memcpy(xmalloc(sizeof(Token)), &tok, sizeof(Token)));
            }
        }
    }
    
    Token eof_tok = lexer_make_token(lexer, TOK_EOF, "", 0);
    array_push(lexer->tokens, memcpy(xmalloc(sizeof(Token)), &eof_tok, sizeof(Token)));
}

Token *lexer_get_tokens(Lexer *lexer, size_t *count) {
    *count = lexer->tokens->count;
    return (Token *)lexer->tokens->items;
}

void lexer_free(Lexer *lexer) {
    for (size_t i = 0; i < lexer->tokens->count; i++) {
        xfree(array_get(lexer->tokens, i));
    }
    array_free(lexer->tokens);
    xfree(lexer);
}
