#ifndef WHITE_POLYMORPHIC_H
#define WHITE_POLYMORPHIC_H

#include "common.h"
#include "ast.h"

/* Polymorphic Code Generation Feature
 * 
 * This feature is restricted to legitimate RF engineering, scientific research,
 * communications research, SDR development, and defensive security research.
 * 
 * It is NOT intended for malware, persistence, credential theft, evasion,
 * or any destructive behavior.
 */

typedef enum {
    POLY_INSTRUCTION_REORDER,    /* Reorder independent instructions */
    POLY_REGISTER_REASSIGNMENT,  /* Use different registers each build */
    POLY_DEAD_CODE_INSERTION,    /* Insert harmless dead code */
    POLY_NOP_INJECTION,          /* Insert NOP instructions */
    POLY_FUNCTION_INLINING,      /* Inline/outline functions */
    POLY_CONSTANT_OBFUSCATION,   /* Encode constants differently */
    POLY_ARITHMETIC_TRANSFORMATION, /* x+0 -> x, x*1 -> x equivalences */
} PolymorphicTransform;

typedef struct {
    uint32_t seed;              /* Random seed for reproducibility */
    bool enable_instruction_reorder;
    bool enable_dead_code;
    bool enable_nop_injection;
    bool enable_function_inlining;
    bool enable_constant_obfuscation;
    bool enable_arithmetic_transform;
    int obfuscation_level;      /* 0 (none) to 3 (maximum) */
} PolymorphicOptions;

typedef struct {
    DynamicArray *transformed_code;
    uint32_t transformation_seed;
    int transformations_applied;
} PolymorphicCodeGen;

/* Create polymorphic code generator */
PolymorphicCodeGen *polymorphic_create(PolymorphicOptions *opts);

/* Apply polymorphic transformations to AST */
void polymorphic_transform_ast(PolymorphicCodeGen *pcg, ASTNode *ast);

/* Generate polymorphic C code */
char *polymorphic_emit_c(PolymorphicCodeGen *pcg, ASTNode *ast);

/* Get transformation metadata for verification */
const char *polymorphic_get_metadata(PolymorphicCodeGen *pcg);

/* Cleanup */
void polymorphic_free(PolymorphicCodeGen *pcg);

#endif
