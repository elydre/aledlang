#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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

uint32_t *aled_parse(char *src) {
    uint32_t *code = malloc(1024 * sizeof(uint32_t));
    uint32_t *ptr = code;
    uint32_t size = 1024;

    for (int i = 0; src[i]; i++) {
        if (isspace(src[i])) {
            src[i] = ' ';
        }
    }

    char *tok = strtok(src, " ");
    while (tok) {
        for (aled_kw_t *kw = g_aled_kws; kw->text; kw++) {
            if (strcmp(tok, kw->text))
                continue;
            *ptr++ = kw->key;
            tok = NULL;
            break;
        }
        
        if (tok) {
            uint32_t val = atou_error(tok);
            if (val != UINT32_MAX)
                *ptr++ = val;
            else {
                val = atos_error(tok);
                if (val == UINT32_MAX) {
                    free(code);
                    raise_andexit("Invalid token: %s", tok);
                }
                if (g_jmps[val] != UINT32_MAX) {
                    free(code);
                    raise_andexit("Duplicate label: %s", tok);
                }
                g_jmps[val] = ptr - code;
            }
        }
        if (ptr - code >= size) {
            uint32_t offset = ptr - code;
            size *= 2;
            code = realloc(code, size * sizeof(uint32_t));
            ptr = code + offset;
        }
        tok = strtok(NULL, " ");
    }

    *ptr = UINT32_MAX;
    return code;
}
