#ifndef ALEDLANG_H
#define ALEDLANG_H

#include <stdint.h>

#define ALED_VERSION "2.2.0"

#define JMP_COUNT 512
#define VAL_COUNT 512
#define STACK_SIZE 1024 // don't change compiled output

#define raise_error(fmt, ...) fprintf(stderr, "\e[31mAledLang: Error: " fmt "\e[0m\n", ##__VA_ARGS__);

typedef struct {
    const char *text;
    uint32_t key;
} aled_kw_t;

typedef struct {
    int debug;
    int fast;
    int compile;
    const char *file;
    const char *oneline;
} aled_args_t;

extern aled_kw_t g_aled_kws[];

extern uint32_t *g_jmps;
extern uint32_t *g_vals;

extern uint32_t *g_stack;
extern int g_spos;

#define FIRST_KW (UINT32_MAX - 64)


enum {
    KW_PRINT = FIRST_KW,
    KW_CPUT,
    KW_INPUT,
    KW_CGET,
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


// compiler.c
int aled_compile(FILE *f, uint32_t *code);

// lexer.c
char **aled_lexe(char *src, int *len);
void free_token(char **tokens);

// parser.c
uint32_t *aled_parse(char *src);

// tools.c
char *aled_read_file(const char *file);
void raise_andexit(const char *fmt, ...);
char *aled_read_line(const char *prompt);
void wait_enter(void);

void print_code(uint32_t *code, uint32_t *ptr);
void print_stack(void);

const char *aled_get_kw(uint32_t kw);

uint32_t atou_error(const char *str);
uint32_t atos_error(const char *str);

void aled_cleanup(void);

#ifdef ENABLE_BIN
char *new_temp_file(const char *prefix);
char *del_temp_file(char *file);
int exec_cmd(char **cmd);
#endif

// vm.c
int aled_run(uint32_t *code, int debug);
int aled_run_fast(uint32_t *code);

#endif
