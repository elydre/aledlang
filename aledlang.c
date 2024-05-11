#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#define JMP_COUNT 1024
#define VAL_COUNT 1024
#define STACK_SIZE 1024

enum {
    KW_PRINT = (UINT32_MAX - 64),
    KW_CPUT,
    KW_JIF,
    KW_GOTO,
    KW_SET,
    KW_GET,
    KW_POP,
    KW_ROT,
    KW_DUP,

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

uint32_t *g_jmps;
uint32_t *g_vals;

uint32_t *g_stack;
int g_spos;

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

uint32_t *aled_compile(char *src) {
    uint32_t *code = malloc(1024 * sizeof(uint32_t));
    uint32_t *ptr = code;
    uint32_t size = 1024;

    src = strdup(src);

    char *tok = strtok(src, " ");
    while (tok) {
        if (strcmp(tok, "PRINT") == 0) {
            *ptr++ = KW_PRINT;
        } else if (strcmp(tok, "CPUT") == 0) {
            *ptr++ = KW_CPUT;
        } else if (strcmp(tok, "JIF") == 0) {
            *ptr++ = KW_JIF;
        } else if (strcmp(tok, "GOTO") == 0) {
            *ptr++ = KW_GOTO;
        } else if (strcmp(tok, "SET") == 0) {
            *ptr++ = KW_SET;
        } else if (strcmp(tok, "GET") == 0) {
            *ptr++ = KW_GET;
        } else if (strcmp(tok, "POP") == 0) {
            *ptr++ = KW_POP;
        } else if (strcmp(tok, "ROT") == 0) {
            *ptr++ = KW_ROT;
        } else if (strcmp(tok, "DUP") == 0) {
            *ptr++ = KW_DUP;
        } else if (strcmp(tok, "+") == 0) {
            *ptr++ = OP_ADD;
        } else if (strcmp(tok, "-") == 0) {
            *ptr++ = OP_SUB;
        } else if (strcmp(tok, "*") == 0) {
            *ptr++ = OP_MUL;
        } else if (strcmp(tok, "/") == 0) {
            *ptr++ = OP_DIV;
        } else if (strcmp(tok, "%") == 0) {
            *ptr++ = OP_MOD;
        } else if (strcmp(tok, "==") == 0) {
            *ptr++ = OP_EQ;
        } else if (strcmp(tok, "!=") == 0) {
            *ptr++ = OP_NEQ;
        } else if (strcmp(tok, ">") == 0) {
            *ptr++ = OP_GT;
        } else if (strcmp(tok, "<") == 0) {
            *ptr++ = OP_LT;
        } else if (strcmp(tok, ">=") == 0) {
            *ptr++ = OP_GTE;
        } else if (strcmp(tok, "<=") == 0) {
            *ptr++ = OP_LTE;
        } else {
            uint32_t val = atou_error(tok);
            if (val == UINT32_MAX) {
                val = atos_error(tok);
                if (val == UINT32_MAX) {
                    fprintf(stderr, "Error: invalid token: %s\n", tok);
                    exit(1);
                }
                g_jmps[val] = ptr - code;
            } else {
                *ptr++ = val;
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

    free((char *)src);

    *ptr = UINT32_MAX;
    return code;
}

const char *aled_get_kw(uint32_t kw) {
    switch (kw) {
        case KW_PRINT: return "PRINT";
        case KW_CPUT: return "CPUT";
        case KW_JIF: return "JIF";
        case KW_GOTO: return "GOTO";
        case KW_SET: return "SET";
        case KW_GET: return "GET";
        case KW_POP: return "POP";
        case KW_ROT: return "ROT";
        case KW_DUP: return "DUP";
        case OP_ADD: return "+";
        case OP_SUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_MOD: return "%";
        case OP_EQ: return "==";
        case OP_NEQ: return "!=";
        case OP_GT: return ">";
        case OP_LT: return "<";
        case OP_GTE: return ">=";
        case OP_LTE: return "<=";
        default: return NULL;
    }
    return NULL;
}

void print_code(uint32_t *code, uint32_t *ptr) {
    const char *kw;

    for (uint32_t *p = code; *p != UINT32_MAX; p++) {
        if (p == ptr)
            fputs("\033[1;31m", stdout);
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
    for (int i = 0; i < g_spos; i++) {
        printf("%d ", g_stack[i]);
    }
    printf("\n");
}

void wait_enter(void) {
    while (getchar() != '\n');
}

uint32_t aled_run_error(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    fprintf(stderr, "Stack: ");
    print_stack();
    exit(1);
}

#define POP() (g_spos ? g_stack[--g_spos] : aled_run_error("stack underflow"))

void aled_run(uint32_t *code) {
    uint32_t *ptr = code;
    uint32_t val, idx, a, b;
    while (*ptr != UINT32_MAX) {
        // print_code(code, ptr);
        switch (*ptr) {
            case KW_PRINT:
                printf("%d\n", POP());
                break;
            case KW_CPUT:
                putchar(POP());
                fflush(stdout);
                break;
            case KW_JIF:
                val = POP();
                if (!POP())
                    break;
                ptr = code + g_jmps[val] - 1;
                break;
            case KW_GOTO:
                ptr = code + g_jmps[POP()] - 1;
                break;
            case KW_SET:
                val = POP();
                idx = POP();
                if (idx >= VAL_COUNT) {
                    fprintf(stderr, "Error: invalid index: %d\n", idx);
                    exit(1);
                }
                g_vals[idx] = val;
                break;
            case KW_GET:
                idx = POP();
                if (idx >= VAL_COUNT) {
                    fprintf(stderr, "Error: invalid index: %d\n", idx);
                    exit(1);
                }
                g_stack[g_spos++] = g_vals[idx];
                break;
            case KW_POP:
                POP();
                break;
            case KW_DUP:
                if (g_spos == 0)
                    aled_run_error("stack underflow");
                g_stack[g_spos] = g_stack[g_spos - 1];
                g_spos++;
                break;
            case KW_ROT:
                a = POP();
                b = POP();
                g_stack[g_spos++] = a;
                g_stack[g_spos++] = b;
                break;
            case OP_ADD:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] += g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_SUB:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] -= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_MUL:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] *= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_DIV:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] /= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_MOD:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] %= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_EQ:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] == g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_NEQ:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] != g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_GT:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] > g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_LT:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] < g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_GTE:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] >= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_LTE:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] <= g_stack[g_spos - 1];
                g_spos--;
                break;
            default:
                g_stack[g_spos++] = *ptr;
                break;
        }
        // print_stack();
        // wait_enter();
        ptr++;
    }
}

int main(void) {
    g_jmps = calloc(JMP_COUNT, sizeof(uint32_t));
    g_vals = calloc(VAL_COUNT, sizeof(uint32_t));
    g_stack = calloc(STACK_SIZE, sizeof(uint32_t));
    g_spos = 0;

    uint32_t *code = aled_compile("0 (0) DUP PRINT 1 + DUP 10 < 0 JIF");
    // uint32_t *code = aled_compile("'H' 'e' 'l' 'l' 'o' 10 ");
    aled_run(code);

    free(code);
    free(g_jmps);
    free(g_vals);
    free(g_stack);
    return 0;
}
