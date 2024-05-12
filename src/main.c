#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "aledlang.h"

uint32_t *g_jmps;
uint32_t *g_vals;
uint32_t *g_code;

uint32_t *g_stack;
int g_spos;

void aled_init(void) {
    g_jmps = calloc(JMP_COUNT, sizeof(uint32_t));
    g_vals = calloc(VAL_COUNT, sizeof(uint32_t));
    g_stack = calloc(STACK_SIZE, sizeof(uint32_t));
    g_code = NULL;
    g_spos = 0;
}

void aled_execute(aled_args_t *args) {
    char *src;

    if (args->file) {
        src = aled_read_file(args->file);
    } else {
        src = strdup(args->oneline);
    }

    g_code = aled_parse(src);
    free(src);

    if (!g_code) {
        return;
    }

    if (args->fast) {
        aled_run_fast(g_code);
    } else {
        aled_run(g_code, args->debug);
    }
}

void aled_start_shell(aled_args_t *args) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while (1) {
        printf("aled> ");
        read = getline(&line, &len, stdin);
        if (read == -1) {
            putchar('\n');
            break;
        }
        if (line[0] == '\n') {
            continue;
        }
        g_code = aled_parse(line);
        if (!g_code) {
            continue;
        }
        aled_run(g_code, args->debug);
        free(g_code);
    }
    free(line);
    g_code = NULL;
}



int main(int argc, char **argv) {
    aled_args_t args;

    aled_init();

    args = aled_process_args(argc, argv);

    if (args.fast && args.debug) {
        raise_andexit("Cannot use -f and debug options together");
    }

    if (args.file && args.oneline) {
        raise_andexit("Cannot use file and -c options together");
    }

    if (!args.file && !args.oneline) {
        aled_start_shell(&args);
    } else {
        aled_execute(&args);
    }

    aled_cleanup();
    return 0;
}
