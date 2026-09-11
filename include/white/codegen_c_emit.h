#ifndef WHITE_CODEGEN_C_EMIT_H
#define WHITE_CODEGEN_C_EMIT_H

#include "ast.h"

/* Emit AST as C source code to file */
void codegen_emit_to_c_file(ASTNode *program, const char *output_file);

#endif
