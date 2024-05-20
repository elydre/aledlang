#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "aledlang.h"

uint32_t *g_jmps;
uint32_t *g_vals;

uint32_t *g_stack;
int g_spos;


void aled_init(void) {
    g_vals = calloc(VAL_COUNT, sizeof(uint32_t));
    g_stack = calloc(STACK_SIZE, sizeof(uint32_t));

    g_jmps = malloc(JMP_COUNT * sizeof(uint32_t));
    memset(g_jmps, UINT32_MAX, JMP_COUNT * sizeof(uint32_t));

    g_spos = 0;
}

int aled_execute(aled_args_t *args) {
    uint32_t *code;
    char *src;
    int ret;

    if (args->file) {
        src = aled_read_file(args->file);
    } else {
        src = strdup(args->oneline);
    }

    code = aled_parse(src);
    free(src);

    if (!code)
        return 1;

    if (args->fast) {
        ret = aled_run_fast(code);
    } else {
        ret = aled_run(code, args->debug);
    }

    free(code);
    return ret;
}

int aled_compile_call(aled_args_t *args) {
    uint32_t *code;
    char *src;

    src = aled_read_file(args->file);

    if (!src) {
        return 1;
    }

    code = aled_parse(src);
    free(src);

    if (!code) {
        return 1;
    }

    if (args->compile & 1) {
        aled_compile(stdout, code);
    }

    if (!(args->compile & 2)) {
        free(code);
        return 0;
    }

#ifdef ENABLE_BIN
    // create a temporary file to store the assembly code
    char *tmpasm, *tmpout;
    if (args->compile & 4)
        tmpout = new_temp_file("/tmp/aled-");    
    tmpasm = new_temp_file("/tmp/aled-");

    FILE *f = fopen(tmpasm, "w");
    if (!f) {
        raise_andexit("Failed to create temporary file");
    }

    aled_compile(f, code);
    fclose(f);

    int status = exec_cmd((char *[]) {
        "/bin/gcc",
        "-no-pie",
        "-m32",
        "-x",
        "assembler",
        tmpasm,
        "-o",
        args->compile & 4 ? tmpout : "a.out",
        NULL
    });

    if (status == 0 && args->compile & 4) {
        exec_cmd((char *[]) {
            tmpout,
            NULL
        });
    }

    if (args->compile & 4)
        del_temp_file(tmpout);
    del_temp_file(tmpasm);

    free(code);
    return status;
#else
    raise_andexit("Binary compilation is disabled");
    return 1;
#endif
}

int aled_start_shell(aled_args_t *args) {
    char *line = NULL;
    uint32_t *code;

    while (1) {
        // cleanup jump table
        memset(g_jmps, UINT32_MAX, JMP_COUNT * sizeof(uint32_t));
        line = aled_read_line("aled ~ ");
        if (!line)
            break;
        code = aled_parse(line);
        free(line);
        if (!code)
            continue;
        if (aled_run(code, args->debug))
            g_spos = 0;
        free(code);
        if (!g_spos)
            continue;
        printf("\e[90m[ ");
        for (int i = 0; i < g_spos; i++) {
            printf("%d ", g_stack[i]);
        }
        printf("]\e[0m\n");
    }
    return 0;
}

int main(int argc, char **argv) {
    aled_args_t args;
    int ret = 0;

    aled_init();

    args = aled_process_args(argc, argv);

    if (args.fast && args.debug) {
        raise_andexit("Cannot use -f and debug options together");
    }

    if (args.file && args.oneline) {
        raise_andexit("Cannot use file and -e options together");
    }

    if (!args.file && args.compile) {
        raise_andexit("Cannot use -c/-b without a file");
    }

    if (args.compile) {
        ret = aled_compile_call(&args);
    } else if (!args.file && !args.oneline) {
        ret = aled_start_shell(&args);
    } else {
        ret = aled_execute(&args);
    }

    aled_cleanup();
    return ret;
}
