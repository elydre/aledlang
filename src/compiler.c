#include <stdio.h>

#include "aledlang.h"

typedef struct {
    uint32_t kw;
    const char *asm_code;
} aled_ass_t;

aled_ass_t aled_to_asm[] = {
    {
        KW_PRINT, 
        "# KW_PRINT\n"
        "pushl $print_format\n"
        "call printf\n"
        "addl $8, %esp"
    },
    {
        KW_CPUT,
        "# KW_CPUT\n"
        "pushl $cput_format\n"
        "call printf\n"
        "addl $8, %esp"
    },
    {
        KW_JIF, // TODO
        "# KW_GOTO\n"
        "popl %eax\n"
        "movl $jump_table, %ebx\n"
        "leal (%ebx, %eax, 4), %eax\n"
        "movl (%eax), %eax\n"
        "jmp *%eax"
    },
    {
        KW_GOTO,
        "# KW_GOTO\n"
        "popl %eax\n"
        "movl $jump_table, %ebx\n"
        "leal (%ebx, %eax, 4), %eax\n"
        "movl (%eax), %eax\n"
        "jmp *%eax"
    },
    {KW_SET, NULL},
    {KW_GET, NULL},
    {
        KW_POP,
        "# KW_POP\n"
        "popl %eax"
    },
    {
        KW_SWAP,
        "# KW_SWAP\n"
        "popl %eax\n"
        "popl %ebx\n"
        "pushl %eax\n"
        "pushl %ebx"
    },
    {
        KW_SWAP3,
        "# KW_SWAP3\n"
        "movl 8(%esp), %eax\n"
        "movl 0(%esp), %ebx\n"
        "movl %eax, 0(%esp)\n"
        "movl %ebx, 8(%esp)"
    },
    {KW_ROT, NULL},
    {
        KW_DUP,
        "# KW_DUP\n"
        "pushl (%esp)"
    },
    {
        KW_DUP2,
        "# KW_DUP2\n"
        "pushl 4(%esp)\n"
        "pushl 4(%esp)"
    },
    {
        OP_ADD,
        "# OP_ADD\n"
        "popl %eax\n"
        "popl %ebx\n"
        "addl %eax, %ebx\n"
        "pushl %ebx"
    },
    {
        OP_SUB,
        "# OP_SUB\n"
        "popl %eax\n"
        "popl %ebx\n"
        "subl %eax, %ebx\n"
        "pushl %ebx"
    },
    {
        OP_MUL,
        "# OP_MUL\n"
        "popl %eax\n"
        "popl %ebx\n"
        "imul %eax, %ebx\n"
        "pushl %ebx"
    },
    {
        OP_DIV,
        "# OP_DIV\n"
        "popl %ebx\n"
        "popl %eax\n"
        "cltd\n" // sign extend eax to edx:eax
        "idiv %ebx\n"
        "pushl %eax"
    },
    {
        OP_MOD,
        "# OP_MOD\n"
        "popl %ebx\n"
        "popl %eax\n"
        "cltd\n" // sign extend eax to edx:eax
        "idiv %ebx\n"
        "pushl %edx"
    },
    {
        OP_EQ,
        "# OP_EQ\n"
        "popl %eax\n"
        "popl %ebx\n"
        "cmpl %ebx, %eax\n"
        "movl $0, %ecx\n"
        "movl $1, %edx\n"
        "cmove %edx, %ecx\n"
        "pushl %ecx"
    },
    {
        OP_NEQ,
        "# OP_NEQ\n"
        "popl %eax\n"
        "popl %ebx\n"
        "cmpl %ebx, %eax\n"
        "movl $0, %ecx\n"
        "movl $1, %edx\n"
        "cmovne %edx, %ecx\n"
        "pushl %ecx"
    },
    {
        OP_GT,
        "# OP_GT\n"
        "popl %eax\n"
        "popl %ebx\n"
        "cmpl %ebx, %eax\n"
        "movl $0, %ecx\n"
        "movl $1, %edx\n"
        "cmovl %edx, %ecx\n"
        "pushl %ecx"
    },
    {
        OP_LT,
        "# OP_LT\n"
        "popl %eax\n"
        "popl %ebx\n"
        "cmpl %ebx, %eax\n"
        "movl $0, %ecx\n"
        "movl $1, %edx\n"
        "cmovg %edx, %ecx\n"
        "pushl %ecx"
    },
    {
        OP_GTE,
        "# OP_GTE\n"
        "popl %eax\n"
        "popl %ebx\n"
        "cmpl %ebx, %eax\n"
        "movl $0, %ecx\n"
        "movl $1, %edx\n"
        "cmovle %edx, %ecx\n"
        "pushl %ecx"
    },
    {
        OP_LTE,
        "# OP_LTE\n"
        "popl %eax\n"
        "popl %ebx\n"
        "cmpl %ebx, %eax\n"
        "movl $0, %ecx\n"
        "movl $1, %edx\n"
        "cmovge %edx, %ecx\n"
        "pushl %ecx"
    },
    {0, NULL}
};

void aled_compile(FILE *f, uint32_t *code) {
    uint32_t *ptr;
    int found;

    fprintf(f,
        ".section .data\n"
        "print_format:\n"
        "  .asciz \"%%u\\n\"\n"
        "cput_format:\n"
        "  .asciz \"%%c\"\n"
        "jump_table:\n"
        "  .zero %u\n\n"
        ".section .text\n"
        ".globl main\n\n"
        "main:\n\n",
        JMP_COUNT * 4
    );

    for (int i = 0; i < JMP_COUNT; i++) {
        if (g_jmps[i] == UINT32_MAX)
            continue;
        fprintf(f,
            "# tab %u\n"
            "movl $jmp%u, %%eax\n"
            "movl $jump_table+%u, %%ebx\n"
            "movl %%eax, (%%ebx)\n\n",
            i, i, i * 4
        );
    }

    for (ptr = code ;*ptr != UINT32_MAX; ptr++) {
        for (int i = 0; i < JMP_COUNT; i++) {
            if (g_jmps[i] == (ptr - code)) {
                fprintf(f, "jmp%u:\n", i);
            }
        }
        found = 0;
        for (int i = 0; aled_to_asm[i].kw; i++) {
            if (aled_to_asm[i].kw != *ptr)
                continue;
            found = 1;
            if (aled_to_asm[i].asm_code == NULL) {
                fprintf(stderr, "Warning: Unimplemented keyword %s\n", aled_get_kw(*ptr));
                continue;
            }
            fprintf(f, "%s\n\n", aled_to_asm[i].asm_code);
            break;
        }

        if (found)
            continue;

        if (ptr[1] == KW_GOTO) {
            fprintf(f, "# KW_GOTO (fast)\n"
                "jmp jmp%u\n\n", *ptr);
            ptr++;
            continue;
        }

        if (ptr[1] == KW_JIF) {
            fprintf(f, "# KW_JIF (fast)\n"
                "popl %%eax\n"
                "test %%eax, %%eax\n"
                "jnz jmp%u\n\n", *ptr);
            ptr++;
            continue;
        }

        fprintf(f, "# %u\npushl $%u\n\n", *ptr, *ptr);   
    }

    for (int i = 0; i < JMP_COUNT; i++) {
        if (g_jmps[i] == (ptr - code)) {
            fprintf(f, "jmp%u:\n", i);
        }
    }

    fputs(
        "# call exit\n"
        "pushl $0\n"
        "call exit\n",
        f
    );
}
