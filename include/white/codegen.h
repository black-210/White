#ifndef WHITE_CODEGEN_H
#define WHITE_CODEGEN_H

#include "common.h"
#include "ast.h"
#include "types.h"

typedef struct {
    DynamicArray *output;  /* Generated C lines */
    int indent_level;
    int temp_counter;
} CodeGenerator;

CodeGenerator *codegen_create(void);
void codegen_emit(CodeGenerator *cg, ASTNode *program);
char *codegen_get_output(CodeGenerator *cg);
void codegen_free(CodeGenerator *cg);

#endif
