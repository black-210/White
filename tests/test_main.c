#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <complex.h>

#include "../include/white/common.h"
#include "../include/white/lexer.h"
#include "../include/white/parser.h"
#include "../include/white/ast.h"
#include "../include/white/types.h"
#include "../include/white/semantic.h"
#include "../include/white/diagnostics.h"
#include "../include/white/codegen.h"
#include "../include/white/runtime.h"

/* Test counter */
static int test_count = 0;
static int test_pass = 0;
static int test_fail = 0;

#define TEST_START(name) do { \
    test_count++; \
    printf("\n[Test %d] %s\n", test_count, name); \
} while(0)

#define TEST_ASSERT(cond, msg) do { \
    if (cond) { \
        printf("  ✓ PASS: %s\n", msg); \
        test_pass++; \
    } else { \
        printf("  ✗ FAIL: %s\n", msg); \
        test_fail++; \
    } \
} while(0)

/* Lexer tests */
void test_lexer_numbers(void) {
    TEST_START("Lexer: Number tokenization");
    
    const char *source = "42 3.14 1e10 0x1F";
    Lexer *lexer = lexer_create(source, "test.white");
    lexer_scan_tokens(lexer);
    
    size_t count = 0;
    Token *tokens = lexer_get_tokens(lexer, &count);
    
    TEST_ASSERT(count >= 4, "Tokenized numbers");
    TEST_ASSERT(tokens[0].type == TOK_INT, "First token is INT");
    TEST_ASSERT(tokens[1].type == TOK_FLOAT, "Second token is FLOAT");
    
    lexer_free(lexer);
}

void test_lexer_keywords(void) {
    TEST_START("Lexer: Keyword recognition");
    
    const char *source = "fn if else while return";
    Lexer *lexer = lexer_create(source, "test.white");
    lexer_scan_tokens(lexer);
    
    size_t count = 0;
    Token *tokens = lexer_get_tokens(lexer, &count);
    
    TEST_ASSERT(tokens[0].type == TOK_FN, "fn keyword recognized");
    TEST_ASSERT(tokens[1].type == TOK_IF, "if keyword recognized");
    TEST_ASSERT(tokens[2].type == TOK_ELSE, "else keyword recognized");
    
    lexer_free(lexer);
}

void test_lexer_operators(void) {
    TEST_START("Lexer: Operator tokenization");
    
    const char *source = "+ - * / == != < >";
    Lexer *lexer = lexer_create(source, "test.white");
    lexer_scan_tokens(lexer);
    
    size_t count = 0;
    Token *tokens = lexer_get_tokens(lexer, &count);
    
    TEST_ASSERT(tokens[0].type == TOK_PLUS, "+ operator recognized");
    TEST_ASSERT(tokens[2].type == TOK_STAR, "* operator recognized");
    TEST_ASSERT(tokens[4].type == TOK_EQ, "== operator recognized");
    
    lexer_free(lexer);
}

/* Parser tests */
void test_parser_literals(void) {
    TEST_START("Parser: Literal expressions");
    
    const char *source = "42";
    Lexer *lexer = lexer_create(source, "test.white");
    lexer_scan_tokens(lexer);
    size_t count = 0;
    Token *tokens = lexer_get_tokens(lexer, &count);
    
    Parser *parser = parser_create(tokens, count, "test.white");
    ASTNode *ast = parser_parse(parser);
    
    TEST_ASSERT(ast != NULL, "AST created");
    TEST_ASSERT(ast->type == AST_PROGRAM, "Program node created");
    
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);
}

void test_parser_binary_op(void) {
    TEST_START("Parser: Binary operations");
    
    const char *source = "1 + 2";
    Lexer *lexer = lexer_create(source, "test.white");
    lexer_scan_tokens(lexer);
    size_t count = 0;
    Token *tokens = lexer_get_tokens(lexer, &count);
    
    Parser *parser = parser_create(tokens, count, "test.white");
    ASTNode *ast = parser_parse(parser);
    
    TEST_ASSERT(ast != NULL, "Binary operation parsed");
    
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);
}

/* Type system tests */
void test_types_primitive(void) {
    TEST_START("Type System: Primitive types");
    
    WhiteType *t_i64 = type_create_primitive(TYPE_I64);
    WhiteType *t_f64 = type_create_primitive(TYPE_F64);
    
    TEST_ASSERT(type_is_integer(t_i64), "i64 is integer");
    TEST_ASSERT(type_is_float(t_f64), "f64 is float");
    TEST_ASSERT(type_is_numeric(t_i64), "i64 is numeric");
    TEST_ASSERT(!type_equals(t_i64, t_f64), "i64 != f64");
    
    type_free(t_i64);
    type_free(t_f64);
}

void test_types_rf_types(void) {
    TEST_START("Type System: RF/DSP types");
    
    WhiteType *t_freq = type_create_primitive(TYPE_FREQUENCY);
    WhiteType *t_signal = type_create_primitive(TYPE_SIGNAL);
    WhiteType *t_spectrum = type_create_primitive(TYPE_SPECTRUM);
    
    TEST_ASSERT(type_is_rf_type(t_freq), "frequency is RF type");
    TEST_ASSERT(type_is_rf_type(t_signal), "signal is RF type");
    TEST_ASSERT(type_is_rf_type(t_spectrum), "spectrum is RF type");
    
    type_free(t_freq);
    type_free(t_signal);
    type_free(t_spectrum);
}

void test_types_arrays(void) {
    TEST_START("Type System: Array types");
    
    WhiteType *t_elem = type_create_primitive(TYPE_F64);
    WhiteType *t_array = type_create_array(t_elem, 1024);
    
    TEST_ASSERT(t_array->kind == TYPE_ARRAY, "Array type created");
    TEST_ASSERT(t_array->array_size == 1024, "Array size correct");
    
    type_free(t_array);
}

/* Semantic analysis tests */
void test_semantic_basic(void) {
    TEST_START("Semantic Analysis: Basic");
    
    SemanticAnalyzer *sa = semantic_create();
    
    TEST_ASSERT(sa != NULL, "Semantic analyzer created");
    
    semantic_free(sa);
}

/* Runtime tests */
void test_runtime_complex(void) {
    TEST_START("Runtime: Complex number arithmetic");
    
    white_complex a = 3.0 + 4.0 * I;
    white_complex b = 1.0 + 2.0 * I;
    
    double mag_a = white_complex_magnitude(a);
    TEST_ASSERT(mag_a > 4.9 && mag_a < 5.1, "Complex magnitude correct (5.0)");
    
    white_complex conj_a = white_complex_conj(a);
    TEST_ASSERT(creal(conj_a) == 3.0, "Complex conjugate real part");
    TEST_ASSERT(cimag(conj_a) == -4.0, "Complex conjugate imag part");
}

void test_runtime_iq(void) {
    TEST_START("Runtime: IQ sample handling");
    
    white_iq sample;
    sample.i = 0.7071;
    sample.q = 0.7071;
    
    white_complex c = white_iq_to_complex(sample);
    white_iq sample2 = white_iq_from_complex(c);
    
    TEST_ASSERT(sample2.i > 0.7 && sample2.i < 0.72, "IQ I component preserved");
    TEST_ASSERT(sample2.q > 0.7 && sample2.q < 0.72, "IQ Q component preserved");
}

void test_runtime_signal(void) {
    TEST_START("Runtime: Signal creation and manipulation");
    
    white_signal *s = white_signal_create(1024, 10e6);
    
    TEST_ASSERT(s != NULL, "Signal created");
    TEST_ASSERT(s->length == 1024, "Signal length correct");
    TEST_ASSERT(s->sample_rate == 10e6, "Sample rate correct");
    
    white_signal_free(s);
}

void test_runtime_spectrum(void) {
    TEST_START("Runtime: Spectrum creation");
    
    white_spectrum *sp = white_spectrum_create(512, 10e6);
    
    TEST_ASSERT(sp != NULL, "Spectrum created");
    TEST_ASSERT(sp->length == 512, "Spectrum size correct");
    TEST_ASSERT(sp->magnitude != NULL, "Magnitude array allocated");
    TEST_ASSERT(sp->frequency != NULL, "Frequency array allocated");
    
    white_spectrum_free(sp);
}

int main(void) {
    printf("\n=== White Compiler Test Suite ===\n");
    
    /* Lexer tests */
    test_lexer_numbers();
    test_lexer_keywords();
    test_lexer_operators();
    
    /* Parser tests */
    test_parser_literals();
    test_parser_binary_op();
    
    /* Type system tests */
    test_types_primitive();
    test_types_rf_types();
    test_types_arrays();
    
    /* Semantic analysis tests */
    test_semantic_basic();
    
    /* Runtime tests */
    test_runtime_complex();
    test_runtime_iq();
    test_runtime_signal();
    test_runtime_spectrum();
    
    /* Summary */
    printf("\n=== Test Summary ===\n");
    printf("Total: %d, Passed: %d, Failed: %d\n", test_count, test_pass, test_fail);
    printf("Pass rate: %.1f%%\n", 100.0 * test_pass / test_count);
    
    return test_fail > 0 ? 1 : 0;
}
