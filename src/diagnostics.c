#include "../include/white/diagnostics.h"
#include <stdarg.h>
#include <string.h>

DiagnosticCollector *diag_create(void) {
    DiagnosticCollector *dc = xmalloc(sizeof(DiagnosticCollector));
    dc->diagnostics = array_create();
    dc->error_count = 0;
    dc->warning_count = 0;
    return dc;
}

static void diag_add(DiagnosticCollector *dc, DiagnosticLevel level, const char *filename, int line, int col, const char *fmt, va_list args) {
    Diagnostic *d = xmalloc(sizeof(Diagnostic));
    d->level = level;
    d->filename = filename;
    d->line = line;
    d->column = col;
    
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    d->message = str_dup(buffer);
    d->context_line = NULL;
    d->context_start = 0;
    d->context_length = 0;
    
    array_push(dc->diagnostics, d);
    
    if (level == DIAG_ERROR) dc->error_count++;
    else if (level == DIAG_WARNING) dc->warning_count++;
}

void diag_error(DiagnosticCollector *dc, const char *filename, int line, int col, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    diag_add(dc, DIAG_ERROR, filename, line, col, fmt, args);
    va_end(args);
}

void diag_warning(DiagnosticCollector *dc, const char *filename, int line, int col, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    diag_add(dc, DIAG_WARNING, filename, line, col, fmt, args);
    va_end(args);
}

void diag_note(DiagnosticCollector *dc, const char *filename, int line, int col, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    diag_add(dc, DIAG_NOTE, filename, line, col, fmt, args);
    va_end(args);
}

void diag_print(DiagnosticCollector *dc) {
    for (size_t i = 0; i < dc->diagnostics->count; i++) {
        Diagnostic *d = (Diagnostic *)array_get(dc->diagnostics, i);
        
        const char *level_str = "note";
        if (d->level == DIAG_ERROR) level_str = "error";
        else if (d->level == DIAG_WARNING) level_str = "warning";
        
        fprintf(stderr, "%s:%d:%d: %s: %s\n", d->filename, d->line, d->column, level_str, d->message);
    }
}

bool diag_has_errors(DiagnosticCollector *dc) {
    return dc->error_count > 0;
}

void diag_free(DiagnosticCollector *dc) {
    for (size_t i = 0; i < dc->diagnostics->count; i++) {
        Diagnostic *d = (Diagnostic *)array_get(dc->diagnostics, i);
        xfree(d->message);
        xfree(d->context_line);
        xfree(d);
    }
    array_free(dc->diagnostics);
    xfree(dc);
}
