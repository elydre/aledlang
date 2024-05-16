#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "aledlang.h"

aled_kw_t g_aled_kws[] = {
    {"PRINT", KW_PRINT},
    {"CPUT",  KW_CPUT},
    {"INPUT", KW_INPUT},
    {"CGET",  KW_CGET},
    {"JIF",   KW_JIF},
    {"GOTO",  KW_GOTO},
    {"SET",   KW_SET},
    {"GET",   KW_GET},
    {"POP",   KW_POP},
    {"SWAP",  KW_SWAP},
    {"SWAP3", KW_SWAP3},
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

uint32_t *aled_parse(char *src) {
    uint32_t *code, *ptr;
    char **tokens;
    int len;

    tokens = aled_lexe(src, &len);

    ptr = code = malloc((len + 1) * sizeof(uint32_t));

    for (int i = 0; i < len; i++) {
        char *tok = tokens[i];
        for (aled_kw_t *kw = g_aled_kws; kw->text; kw++) {
            if (strcmp(tok, kw->text))
                continue;
            *ptr++ = kw->key;
            tok = NULL;
            break;
        }

        if (!tok)
            continue;
        uint32_t val = atou_error(tok);
        if (val != UINT32_MAX)
            *ptr++ = val;
        else {
            val = atos_error(tok);
            if (val == UINT32_MAX) {
                fprintf(stderr, "AledLang: Error: Invalid token: %s\n", tok);
                free_token(tokens);
                free(code);
                raise_andexit(NULL);
            }
            if (g_jmps[val] != UINT32_MAX) {
                fprintf(stderr, "AledLang: Error: Invalid token: %s\n", tok);
                free_token(tokens);
                free(code);
                raise_andexit(NULL);
            }
            g_jmps[val] = ptr - code;
        }
    }

    free_token(tokens);

    *ptr = UINT32_MAX;
    return code;
}
