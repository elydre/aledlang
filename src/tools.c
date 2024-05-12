#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "aledlang.h"

char *aled_read_file(const char *file) {
    FILE *f = fopen(file, "r");
    if (!f) {
        raise_andexit("Could not open file: %s", file);
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(size + 1);
    if (!buf || fread(buf, 1, size, f) != size) {
        fclose(f);
        free(buf);
        raise_andexit("Could not read file: %s", file);
    }
    buf[size] = '\0';

    fclose(f);

    return buf;
}

void wait_enter(void) {
    while (getchar() != '\n');
}

void print_code(uint32_t *ptr) {
    const char *kw;

    for (uint32_t *p = g_code; *p != UINT32_MAX; p++) {
        if (p == ptr)
            fputs("\033[31m", stdout);
        kw = aled_get_kw(*p);
        if (kw) printf("%s", kw);
        else printf("%d", *p);
        if (p == ptr)
            fputs("\033[0m", stdout);
        putchar(' ');
    }
    printf("\n");
}

void print_stack(void) {
    if (!g_spos) {
        printf("empty\n");
        return;
    }

    printf("[ ");
    for (int i = 0; i < g_spos; i++) {
        printf("%d ", g_stack[i]);
    }
    printf("]\n");
}

const char *aled_get_kw(uint32_t kw) {
    for (aled_kw_t *k = g_aled_kws; k->text; k++) {
        if (k->key == kw)
            return k->text;
    }
    return NULL;
}

uint32_t atou_error(const char *str) {
    uint32_t val = 0;
    if (*str == '\'' && str[1] && str[2] == '\'') {
        return str[1];
    }
    while (*str) {
        if (*str < '0' || *str > '9') {
            return UINT32_MAX;
        }
        val = val * 10 + (*str - '0');
        str++;
    }
    if (val > UINT32_MAX - 64)
        return UINT32_MAX;
    return val;
}

uint32_t atos_error(const char *str) {
    uint32_t val = 0;
    if (str[0] != '(')
        return UINT32_MAX;
    str++;
    while (*str) {
        if (*str == ')') {
            str++;
            return *str ? UINT32_MAX : val;
        }
        if (*str < '0' || *str > '9') {
            return UINT32_MAX;
        }
        val = val * 10 + (*str - '0');
        str++;
    }
    return val;
}

void aled_cleanup(void) {
    free(g_stack);
    free(g_jmps);
    free(g_vals);
    free(g_code);
    free(g_src);
}

void raise_andexit(const char *fmt, ...) {
    va_list args;
    if (fmt) {
        va_start(args, fmt);
        fprintf(stderr, "AledLang: Error: ");
        vfprintf(stderr, fmt, args);
        fprintf(stderr, "\n");
        va_end(args);
    }
    aled_cleanup();
    exit(1);
}
