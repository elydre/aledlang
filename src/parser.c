#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "aledlang.h"

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
