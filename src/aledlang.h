#ifndef ALEDLANG_H
#define ALEDLANG_H

#include <stdint.h>

#define ALED_VERSION "1.0.0"

#define JMP_COUNT 1024
#define VAL_COUNT 1024
#define STACK_SIZE 1024

typedef struct {
    const char *text;
    uint32_t key;
} aled_kw_t;

typedef struct {
    int debug;
    int fast;
    const char *file;
    const char *oneline;
} aled_args_t;

extern aled_kw_t g_aled_kws[];

extern uint32_t *g_jmps;
extern uint32_t *g_vals;
extern uint32_t *g_code;

extern uint32_t *g_stack;
extern char *g_src;
extern int g_spos;

enum {
    KW_PRINT = (UINT32_MAX - 64),
    KW_CPUT,
    KW_JIF,
    KW_GOTO,
    KW_SET,
    KW_GET,
    KW_POP,
    KW_SWAP,
    KW_SWAP3,
    KW_ROT,
    KW_DUP,
    KW_DUP2,

    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EQ,
    OP_NEQ,
    OP_GT,
    OP_LT,
    OP_GTE,
    OP_LTE,
};

// args.c
aled_args_t aled_process_args(int argc, char **argv);


// lexer.c
char **aled_lexe(char *src, int *len);
void free_token(char **tokens);


// parser.c
uint32_t *aled_parse(char *src);


// tools.c
char *aled_read_file(const char *file);
void raise_andexit(const char *fmt, ...);
void wait_enter(void);

void print_code(uint32_t *ptr);
void print_stack(void);

const char *aled_get_kw(uint32_t kw);

uint32_t atou_error(const char *str);
uint32_t atos_error(const char *str);

void aled_cleanup(void);


// vm.c
void aled_run(uint32_t *code, int debug);
void aled_run_fast(uint32_t *code);


#endif
