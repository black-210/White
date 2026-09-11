#ifndef WHITE_COMPILER_H
#define WHITE_COMPILER_H

#include "common.h"

typedef enum {
    COMPILE_RUN,
    COMPILE_BUILD,
    COMPILE_CHECK,
    COMPILE_EMIT_C,
} CompileMode;

typedef struct {
    const char *input_file;
    const char *output_file;
    CompileMode mode;
    bool verbose;
    bool emit_ir;
} CompileOptions;

int compiler_compile(CompileOptions *opts);
char *compiler_version(void);

#endif
