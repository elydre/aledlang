#include <setjmp.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include "aledlang.h"

jmp_buf g_jmpbuf;

void aled_run_error(uint32_t *code, uint32_t *ptr, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    fprintf(stderr, "\e[31mAledLang: RunTime Error: ");
    vfprintf(stderr, fmt, args);

    fprintf(stderr, "\e[0m\n========================\n");
    fprintf(stderr, "INSTRUCTION:\n  ");
    print_code(code, ptr);
    fprintf(stderr, "STACK:\n  ");
    print_stack();
    fprintf(stderr, "========================\n");

    va_end(args);

    longjmp(g_jmpbuf, 1);
}

#define POP() (g_spos ? g_stack[--g_spos] : (aled_run_error(code, ptr, "Stack underflow"), 0))

int aled_run(uint32_t *code, int debug) {
    uint32_t val, idx, a, b;
    uint32_t *ptr = code;

    if (setjmp(g_jmpbuf))
        return 1;

    while (*ptr != UINT32_MAX) {
        if (debug)
            print_code(code, ptr);
        switch (*ptr) {
            case KW_PRINT:
                printf("\e[92m%u\e[0m\n", POP());
                break;
            case KW_CPUT:
                printf("\e[92m%c\e[0m", POP());
                fflush(stdout);
                break;
            case KW_INPUT:
                scanf("%u", &g_stack[g_spos++]);
                break;
            case KW_CGET:
                g_stack[g_spos++] = getchar();
                break;
            case KW_JIF:
                val = POP();
                if (!POP())
                    break;
                if (g_jmps[val] == UINT32_MAX)
                    aled_run_error(code, ptr, "Jump to non-existent label: %d", val);
                ptr = code + g_jmps[val] - 1;
                break;
            case KW_GOTO:
                val = POP();
                if (g_jmps[val] == UINT32_MAX)
                    aled_run_error(code, ptr, "Jump to non-existent label: %d", val);
                ptr = code + g_jmps[val] - 1;
                break;
            case KW_SET:
                idx = POP();
                val = POP();
                if (idx >= VAL_COUNT)
                    aled_run_error(code, ptr, "Invalid index: %d", idx);
                if (debug)
                    printf("SET: mem[%d] <= %d\n", idx, val);
                g_vals[idx] = val;
                break;
            case KW_GET:
                idx = POP();
                if (idx >= VAL_COUNT)
                    aled_run_error(code, ptr, "Invalid index: %d", idx);
                if (debug)
                    printf("GET: mem[%d] => %d\n", idx, g_vals[idx]);
                g_stack[g_spos++] = g_vals[idx];
                break;
            case KW_POP:
                POP();
                break;
            case KW_DUP:
                if (g_spos == 0)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos] = g_stack[g_spos - 1];
                g_spos++;
                break;
            case KW_DUP2:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                if (g_spos >= STACK_SIZE - 2)
                    aled_run_error(code, ptr, "Stack overflow");
                g_stack[g_spos] = g_stack[g_spos - 2];
                g_stack[g_spos + 1] = g_stack[g_spos - 1];
                g_spos += 2;
                break;
            case KW_SWAP:
                a = POP();
                b = POP();
                g_stack[g_spos++] = a;
                g_stack[g_spos++] = b;
                break;
            case KW_SWAP3:
                if (g_spos < 3)
                    aled_run_error(code, ptr, "Stack underflow");
                a = g_stack[g_spos - 1];
                g_stack[g_spos - 1] = g_stack[g_spos - 3];
                g_stack[g_spos - 3] = a;
                break;
            case KW_ROT:
                if (g_spos < 2)
                    break;
                a = g_stack[g_spos - 1];
                for (int i = g_spos - 1; i > 0; i--)
                    g_stack[i] = g_stack[i - 1];
                g_stack[0] = a;
                break;
            case OP_ADD:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] += g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_SUB:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] -= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_MUL:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] *= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_DIV:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] /= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_MOD:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] %= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_EQ:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] == g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_NEQ:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] != g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_GT:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] > g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_LT:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] < g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_GTE:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] >= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_LTE:
                if (g_spos < 2)
                    aled_run_error(code, ptr, "Stack underflow");
                g_stack[g_spos - 2] = g_stack[g_spos - 2] <= g_stack[g_spos - 1];
                g_spos--;
                break;
            default:
                g_stack[g_spos++] = *ptr;
                break;
        }
        if (g_spos >= STACK_SIZE)
            aled_run_error(code, ptr, "Stack overflow");
        if (debug)
            print_stack();
        if (debug == 2)
            wait_enter();
        ptr++;
    }

    return 0;
}

int aled_run_fast(uint32_t *code) {
    uint32_t *ptr = code;
    uint32_t a;
    while (*ptr != UINT32_MAX) {
        switch (*ptr) {
            case KW_PRINT:
                printf("\e[92m%u\e[0m\n", g_stack[--g_spos]);
                break;
            case KW_CPUT:
                printf("\e[92m%c\e[0m", g_stack[--g_spos]);
                fflush(stdout);
                break;
            case KW_INPUT:
                scanf("%u", &g_stack[g_spos++]);
                break;
            case KW_CGET:
                g_stack[g_spos++] = getchar();
                break;
            case KW_JIF:
                g_spos -= 2;
                if (!g_stack[g_spos])
                    break;
                ptr = code + g_jmps[g_stack[g_spos + 1]] - 1;
                break;
            case KW_GOTO:
                ptr = code + g_jmps[g_stack[--g_spos]] - 1;
                break;
            case KW_SET:
                g_spos -= 2;
                g_vals[g_stack[g_spos + 1]] = g_stack[g_spos];
                break;
            case KW_GET:
                g_stack[g_spos - 1] = g_vals[g_stack[g_spos - 1]];
                break;
            case KW_POP:
                g_spos--;
                break;
            case KW_DUP:
                g_stack[g_spos] = g_stack[g_spos - 1];
                g_spos++;
                break;
            case KW_DUP2:
                g_stack[g_spos] = g_stack[g_spos - 2];
                g_stack[g_spos + 1] = g_stack[g_spos - 1];
                g_spos += 2;
                break;
            case KW_SWAP:
                a = g_stack[g_spos - 1];
                g_stack[g_spos - 1] = g_stack[g_spos - 2];
                g_stack[g_spos - 2] = a;
                break;
            case KW_SWAP3:
                a = g_stack[g_spos - 1];
                g_stack[g_spos - 1] = g_stack[g_spos - 3];
                g_stack[g_spos - 3] = a;
                break;
            case KW_ROT:
                if (g_spos < 2)
                    break;
                a = g_stack[g_spos - 1];
                for (int i = g_spos - 1; i > 0; i--)
                    g_stack[i] = g_stack[i - 1];
                g_stack[0] = a;
                break;
            case OP_ADD:
                g_spos--;
                g_stack[g_spos - 1] += g_stack[g_spos];
                break;
            case OP_SUB:
                g_spos--;
                g_stack[g_spos - 1] -= g_stack[g_spos];
                break;
            case OP_MUL:
                g_spos--;
                g_stack[g_spos - 1] *= g_stack[g_spos];
                break;
            case OP_DIV:
                g_spos--;
                g_stack[g_spos - 1] /= g_stack[g_spos];
                break;
            case OP_MOD:
                g_spos--;
                g_stack[g_spos - 1] %= g_stack[g_spos];
                break;
            case OP_EQ:
                g_spos--;
                g_stack[g_spos - 1] = g_stack[g_spos - 1] == g_stack[g_spos];
                break;
            case OP_NEQ:
                g_spos--;
                g_stack[g_spos - 1] = g_stack[g_spos - 1] != g_stack[g_spos];
                break;
            case OP_GT:
                g_spos--;
                g_stack[g_spos - 1] = g_stack[g_spos - 1] > g_stack[g_spos];
                break;
            case OP_LT:
                g_spos--;
                g_stack[g_spos - 1] = g_stack[g_spos - 1] < g_stack[g_spos];
                break;
            case OP_GTE:
                g_spos--;
                g_stack[g_spos - 1] = g_stack[g_spos - 1] >= g_stack[g_spos];
                break;
            case OP_LTE:
                g_spos--;
                g_stack[g_spos - 1] = g_stack[g_spos - 1] <= g_stack[g_spos];
                break;
            default:
                g_stack[g_spos++] = *ptr;
                break;
        }
        ptr++;
    }

    return 0;
}
