#include "../include/white/compiler.h"
#include "../include/white/lexer.h"
#include "../include/white/parser.h"
#include "../include/white/semantic.h"
#include "../include/white/codegen.h"
#include "../include/white/diagnostics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Error: could not open file '%s'\n", filename);
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = xmalloc(size + 1);
    fread(content, 1, size, f);
    content[size] = 0;
    fclose(f);
    
    return content;
}

static void write_file(const char *filename, const char *content) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "Error: could not write file '%s'\n", filename);
        return;
    }
    fprintf(f, "%s", content);
    fclose(f);
}

int compiler_compile(CompileOptions *opts) {
    if (!opts || !opts->input_file) {
        fprintf(stderr, "Error: no input file\n");
        return 1;
    }
    
    char *source = read_file(opts->input_file);
    if (!source) return 1;
    
    /* Lexical analysis */
    Lexer *lexer = lexer_create(source, opts->input_file);
    lexer_scan_tokens(lexer);
    size_t token_count = 0;
    Token *tokens = lexer_get_tokens(lexer, &token_count);
    
    if (opts->verbose) {
        fprintf(stderr, "Lexer produced %zu tokens\n", token_count);
    }
    
    /* Parsing */
    Parser *parser = parser_create(tokens, token_count, opts->input_file);
    ASTNode *ast = parser_parse(parser);
    
    if (opts->verbose) {
        fprintf(stderr, "Parser completed\n");
    }
    
    /* Semantic analysis */
    SemanticAnalyzer *semantic = semantic_create();
    semantic_analyze(semantic, ast);
    
    if (opts->verbose) {
        fprintf(stderr, "Semantic analysis completed\n");
    }
    
    /* Code generation */
    if (opts->mode == COMPILE_EMIT_C) {
        CodeGenerator *codegen = codegen_create();
        codegen_emit(codegen, ast);
        
        if (opts->output_file) {
            /* Already printed to stdout, but could save to file */
        }
        
        codegen_free(codegen);
    } else if (opts->mode == COMPILE_CHECK) {
        if (opts->verbose) {
            fprintf(stderr, "Check passed\n");
        }
    } else if (opts->mode == COMPILE_RUN || opts->mode == COMPILE_BUILD) {
        CodeGenerator *codegen = codegen_create();
        codegen_emit(codegen, ast);
        
        char c_file[256];
        if (opts->output_file) {
            snprintf(c_file, sizeof(c_file), "%s.c", opts->output_file);
        } else {
            snprintf(c_file, sizeof(c_file), "%s.c", opts->input_file);
        }
        
        codegen_free(codegen);
        
        if (opts->verbose) {
            fprintf(stderr, "Generated C code\n");
        }
    }
    
    /* Cleanup */
    ast_free(ast);
    parser_free(parser);
    lexer_free(lexer);
    semantic_free(semantic);
    xfree(source);
    
    return 0;
}

char *compiler_version(void) {
    return "White 0.1.0";
}
