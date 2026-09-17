#include "../include/white/compiler.h"
#include "../include/white/lexer.h"
#include "../include/white/parser.h"
#include "../include/white/semantic.h"
#include "../include/white/codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *read_file(const char *name) { FILE *f=fopen(name,"rb"); if(!f){fprintf(stderr,"Error: could not open file '%s'\n",name);return NULL;} if(fseek(f,0,SEEK_END)!=0){fclose(f);return NULL;} long n=ftell(f); if(n<0){fclose(f);return NULL;} rewind(f); char *s=xmalloc((size_t)n+1); size_t got=fread(s,1,(size_t)n,f); fclose(f); s[got]=0; return s; }
static int run_command(const char *cmd) { int rc=system(cmd); return rc==-1?1:rc; }
int compiler_compile(CompileOptions *opts) {
    if(!opts||!opts->input_file)return 1; char *source=read_file(opts->input_file); if(!source)return 1;
    Lexer *lexer=lexer_create(source,opts->input_file); lexer_scan_tokens(lexer); size_t count=0; Token *tokens=lexer_get_tokens(lexer,&count); if(opts->verbose)fprintf(stderr,"Lexer produced %zu tokens\n",count);
    Parser *parser=parser_create(tokens,count,opts->input_file); ASTNode *ast=parser_parse(parser); if(!ast){fprintf(stderr,"Error: parsing failed\n");lexer_free(lexer);xfree(source);return 1;}
    SemanticAnalyzer *semantic=semantic_create(); semantic_analyze(semantic,ast);
    if(opts->mode==COMPILE_CHECK){if(opts->verbose)fprintf(stderr,"Check passed\n"); ast_free(ast);parser_free(parser);lexer_free(lexer);semantic_free(semantic);xfree(source);return 0;}
    CodeGenerator *cg=codegen_create(); codegen_emit(cg,ast);
    if(opts->mode==COMPILE_EMIT_C){codegen_free(cg);ast_free(ast);parser_free(parser);lexer_free(lexer);semantic_free(semantic);xfree(source);return 0;}
    const char *out=opts->output_file?opts->output_file:"a.out"; char cfile[1024]; snprintf(cfile,sizeof(cfile),"%s.white.c",out); FILE *f=fopen(cfile,"w"); if(!f){fprintf(stderr,"Error: cannot write generated C file\n");codegen_free(cg);return 1;} fflush(stdout); fclose(f);
    /* The existing code generator writes to stdout. Capture output through the shell for a portable backend. */
    char command[2048]; snprintf(command,sizeof(command),"%s %s > %s", "true", "", "/dev/null"); (void)command;
    fprintf(stderr,"Error: build backend requires codegen output capture; use 'emit-c' until the backend is upgraded.\n");
    codegen_free(cg);ast_free(ast);parser_free(parser);lexer_free(lexer);semantic_free(semantic);xfree(source);return 1;
}
char *compiler_version(void){return "White 0.2.0";}
