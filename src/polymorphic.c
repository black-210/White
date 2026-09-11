#include "../include/white/polymorphic.h"
#include <time.h>
#include <stdarg.h>

/* Simple pseudo-random number generator for reproducibility */
static uint32_t polymorphic_rand(uint32_t *seed) {
    *seed = (*seed * 1103515245 + 12345) & 0x7fffffff;
    return *seed;
}

/* Dead code patterns - harmless computational stubs */
static const char *dead_code_patterns[] = {
    "double _unused_%d = 0.0;",
    "int _loop_%d = 0; while (_loop_%d < 0) { _loop_%d++; }",
    "if (0) { printf(\"unreachable\"); }",
    "double _x_%d = 1.0; if (_x_%d < 0) { _x_%d = 0.0; }",
};
static const int dead_code_count = sizeof(dead_code_patterns) / sizeof(dead_code_patterns[0]);

PolymorphicCodeGen *polymorphic_create(PolymorphicOptions *opts) {
    PolymorphicCodeGen *pcg = xmalloc(sizeof(PolymorphicCodeGen));
    pcg->transformed_code = array_create();
    
    if (opts) {
        pcg->transformation_seed = opts->seed ? opts->seed : (uint32_t)time(NULL);
    } else {
        pcg->transformation_seed = (uint32_t)time(NULL);
    }
    
    pcg->transformations_applied = 0;
    return pcg;
}

static void polymorphic_insert_dead_code(PolymorphicCodeGen *pcg, int count) {
    for (int i = 0; i < count; i++) {
        int pattern_idx = polymorphic_rand(&pcg->transformation_seed) % dead_code_count;
        const char *pattern = dead_code_patterns[pattern_idx];
        
        char buffer[256];
        snprintf(buffer, sizeof(buffer), pattern, i, i, i, i, i);
        array_push(pcg->transformed_code, str_dup(buffer));
        pcg->transformations_applied++;
    }
}

static void polymorphic_reorder_statements(PolymorphicCodeGen *pcg, ASTNode *block) {
    if (!block || block->type != AST_BLOCK) return;
    
    ASTBlock *b = (ASTBlock *)block->data;
    if (b->statements->count <= 1) return;
    
    /* Simple bubble shuffle: reorder independent statements */
    for (size_t i = 0; i < b->statements->count - 1; i++) {
        size_t swap_idx = i + (polymorphic_rand(&pcg->transformation_seed) % (b->statements->count - i));
        
        void *temp = b->statements->items[i];
        b->statements->items[i] = b->statements->items[swap_idx];
        b->statements->items[swap_idx] = temp;
        
        pcg->transformations_applied++;
    }
}

static char *polymorphic_obfuscate_constant(uint32_t *seed, int64_t constant) {
    /* Encode constant as XOR of random values */
    uint32_t key = polymorphic_rand(seed);
    int64_t obfuscated = constant ^ key;
    
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "((int64_t)%lld ^ %ud)", (long long)obfuscated, key);
    return str_dup(buffer);
}

void polymorphic_transform_ast(PolymorphicCodeGen *pcg, ASTNode *ast) {
    if (!ast || ast->type != AST_PROGRAM) return;
    
    ASTProgram *prog = (ASTProgram *)ast->data;
    
    for (size_t i = 0; i < prog->statements->count; i++) {
        ASTNode *stmt = (ASTNode *)array_get(prog->statements, i);
        
        if (stmt->type == AST_BLOCK) {
            polymorphic_reorder_statements(pcg, stmt);
        } else if (stmt->type == AST_FUNC_DEF) {
            ASTFuncDef *func = (ASTFuncDef *)stmt->data;
            if (func->body) {
                polymorphic_reorder_statements(pcg, func->body);
            }
        }
    }
}

char *polymorphic_emit_c(PolymorphicCodeGen *pcg, ASTNode *ast) {
    if (!pcg) return str_dup("");
    
    /* Apply transformations */
    polymorphic_transform_ast(pcg, ast);
    
    /* Insert dead code based on seed */
    int dead_code_count = 2 + (polymorphic_rand(&pcg->transformation_seed) % 3);
    polymorphic_insert_dead_code(pcg, dead_code_count);
    
    /* Concatenate all generated lines */
    size_t total = 0;
    for (size_t i = 0; i < pcg->transformed_code->count; i++) {
        total += strlen((char *)array_get(pcg->transformed_code, i)) + 1;
    }
    
    char *result = xmalloc(total + 1);
    result[0] = 0;
    for (size_t i = 0; i < pcg->transformed_code->count; i++) {
        strcat(result, (char *)array_get(pcg->transformed_code, i));
        strcat(result, "\n");
    }
    
    return result;
}

const char *polymorphic_get_metadata(PolymorphicCodeGen *pcg) {
    static char buffer[512];
    snprintf(buffer, sizeof(buffer), 
             "Polymorphic Code Generation Metadata:\n"
             "  Seed: 0x%08x\n"
             "  Transformations Applied: %d\n",
             pcg->transformation_seed,
             pcg->transformations_applied);
    return buffer;
}

void polymorphic_free(PolymorphicCodeGen *pcg) {
    if (!pcg) return;
    
    for (size_t i = 0; i < pcg->transformed_code->count; i++) {
        xfree(array_get(pcg->transformed_code, i));
    }
    array_free(pcg->transformed_code);
    xfree(pcg);
}
