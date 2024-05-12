#include <stdlib.h>
#include <stdio.h>

#include "aledlang.h"

aled_kw_t g_aled_kws[] = {
    {"PRINT", KW_PRINT},
    {"CPUT",  KW_CPUT},
    {"JIF",   KW_JIF},
    {"GOTO",  KW_GOTO},
    {"SET",   KW_SET},
    {"GET",   KW_GET},
    {"POP",   KW_POP},
    {"ROT",   KW_ROT},
    {"DUP",   KW_DUP},
    {"DUP2",  KW_DUP2},
    {"+",     OP_ADD},
    {"-",     OP_SUB},
    {"*",     OP_MUL},
    {"/",     OP_DIV},
    {"%",     OP_MOD},
    {"==",    OP_EQ},
    {"!=",    OP_NEQ},
    {">",     OP_GT},
    {"<",     OP_LT},
    {">=",    OP_GTE},
    {"<=",    OP_LTE},
    {NULL, 0},
};

uint32_t *g_jmps;
uint32_t *g_vals;
uint32_t *g_code;

uint32_t *g_stack;
int g_spos;

void aled_init(void) {
    g_jmps = calloc(JMP_COUNT, sizeof(uint32_t));
    g_vals = calloc(VAL_COUNT, sizeof(uint32_t));
    g_stack = calloc(STACK_SIZE, sizeof(uint32_t));
    g_spos = 0;
}

int main(int argc, char **argv) {
    aled_args_t args = aled_process_args(argc, argv);

    if (!args.file) {
        fprintf(stderr, "Error: no file specified\n");
        exit(1);
    }

    if (args.fast && args.debug) {
        fprintf(stderr, "Error: cannot use -f and debug options together\n");
        exit(1);
    }

    aled_init();
    
    char *src = aled_read_file(args.file);
    g_code = aled_parse(src);
    free(src);

    if (args.fast)
        aled_run_fast(g_code);
    else
        aled_run(g_code, args.debug);

    free(g_stack);
    free(g_jmps);
    free(g_vals);
    free(g_code);

    return 0;
}
