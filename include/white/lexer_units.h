#ifndef WHITE_LEXER_UNITS_H
#define WHITE_LEXER_UNITS_H

#include "lexer.h"

/* Parse number with unit suffix (e.g., 2.4GHz, 10MHz) */
double lexer_parse_number_with_unit(const char *lexeme, size_t length, TokenType unit_type);

/* Check if identifier is a unit suffix */
TokenType lexer_check_unit(const char *lexeme, size_t length);

/* Get unit name and multiplier */
const char *lexer_unit_name(TokenType type);
double lexer_unit_multiplier(TokenType type);

#endif
