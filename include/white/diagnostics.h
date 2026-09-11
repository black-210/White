#ifndef WHITE_DIAGNOSTICS_H
#define WHITE_DIAGNOSTICS_H

#include "common.h"

typedef enum {
    DIAG_ERROR,
    DIAG_WARNING,
    DIAG_NOTE,
} DiagnosticLevel;

typedef struct {
    DiagnosticLevel level;
    const char *filename;
    int line;
    int column;
    char *message;
    char *context_line;
    int context_start;
    int context_length;
} Diagnostic;

typedef struct {
    DynamicArray *diagnostics;
    int error_count;
    int warning_count;
} DiagnosticCollector;

DiagnosticCollector *diag_create(void);
void diag_error(DiagnosticCollector *dc, const char *filename, int line, int col, const char *fmt, ...);
void diag_warning(DiagnosticCollector *dc, const char *filename, int line, int col, const char *fmt, ...);
void diag_note(DiagnosticCollector *dc, const char *filename, int line, int col, const char *fmt, ...);
void diag_print(DiagnosticCollector *dc);
bool diag_has_errors(DiagnosticCollector *dc);
void diag_free(DiagnosticCollector *dc);

#endif
