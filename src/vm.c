#include <stdlib.h>
#include <stdio.h>

#include "aledlang.h"

uint32_t aled_run_error(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    fprintf(stderr, "Stack: ");
    print_stack();
    exit(1);
}

#define POP() (g_spos ? g_stack[--g_spos] : aled_run_error("stack underflow"))

void aled_run(uint32_t *code, int debug) {
    uint32_t *ptr = code;
    uint32_t val, idx, a, b;
    while (*ptr != UINT32_MAX) {
        if (debug)
            print_code(ptr);
        switch (*ptr) {
            case KW_PRINT:
                printf("\e[92m%d\e[0m\n", POP());
                break;
            case KW_CPUT:
                printf("\e[92m%c\e[0m", POP());
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
                idx = POP();
                val = POP();
                if (idx >= VAL_COUNT) {
                    fprintf(stderr, "Error: invalid index: %d\n", idx);
                    exit(1);
                }
                if (debug)
                    printf("SET: mem[%d] <= %d\n", idx, val);
                g_vals[idx] = val;
                break;
            case KW_GET:
                idx = POP();
                if (idx >= VAL_COUNT) {
                    fprintf(stderr, "Error: invalid index: %d\n", idx);
                    exit(1);
                }
                if (debug)
                    printf("GET: mem[%d] => %d\n", idx, g_vals[idx]);
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
            case KW_DUP2:
                if (g_spos < 2)
                    aled_run_error("stack underflow");
                if (g_spos >= STACK_SIZE - 2)
                    aled_run_error("stack overflow");
                g_stack[g_spos] = g_stack[g_spos - 2];
                g_stack[g_spos + 1] = g_stack[g_spos - 1];
                g_spos += 2;
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
        if (g_spos >= STACK_SIZE)
            aled_run_error("stack overflow");
        if (debug)
            print_stack();
        if (debug == 2)
            wait_enter();
        ptr++;
    }
}

void aled_run_fast(uint32_t *code) {
    uint32_t *ptr = code;
    uint32_t a;
    while (*ptr != UINT32_MAX) {
        switch (*ptr) {
            case KW_PRINT:
                printf("\e[92m%d\e[0m\n", g_stack[--g_spos]);
                break;
            case KW_CPUT:
                printf("\e[92m%c\e[0m", g_stack[--g_spos]);
                fflush(stdout);
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
                g_vals[g_stack[g_spos - 1]] = g_stack[g_spos - 2];
                g_spos -= 2;
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
            case KW_ROT:
                a = g_stack[g_spos - 1];
                g_stack[g_spos - 1] = g_stack[g_spos - 2];
                g_stack[g_spos - 2] = a;
                break;
            case OP_ADD:
                g_stack[g_spos - 2] += g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_SUB:
                g_stack[g_spos - 2] -= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_MUL:
                g_stack[g_spos - 2] *= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_DIV:
                g_stack[g_spos - 2] /= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_MOD:
                g_stack[g_spos - 2] %= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_EQ:
                g_stack[g_spos - 2] = g_stack[g_spos - 2] == g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_NEQ:
                g_stack[g_spos - 2] = g_stack[g_spos - 2] != g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_GT:
                g_stack[g_spos - 2] = g_stack[g_spos - 2] > g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_LT:
                g_stack[g_spos - 2] = g_stack[g_spos - 2] < g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_GTE:
                g_stack[g_spos - 2] = g_stack[g_spos - 2] >= g_stack[g_spos - 1];
                g_spos--;
                break;
            case OP_LTE:
                g_stack[g_spos - 2] = g_stack[g_spos - 2] <= g_stack[g_spos - 1];
                g_spos--;
                break;
            default:
                g_stack[g_spos++] = *ptr;
                break;
        }
        ptr++;
    }
}