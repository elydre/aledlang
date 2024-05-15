#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "aledlang.h"

#ifdef ENABLE_BIN
#include <sys/wait.h>
#include <unistd.h>
#endif

uint32_t *g_jmps;
uint32_t *g_vals;
uint32_t *g_code;

uint32_t *g_stack;
char *g_src;
int g_spos;

void aled_init(void) {
    g_vals = calloc(VAL_COUNT, sizeof(uint32_t));
    g_stack = calloc(STACK_SIZE, sizeof(uint32_t));

    g_jmps = malloc(JMP_COUNT * sizeof(uint32_t));
    memset(g_jmps, UINT32_MAX, JMP_COUNT * sizeof(uint32_t));

    g_code = NULL;
    g_spos = 0;
}

void aled_execute(aled_args_t *args) {
    if (args->file) {
        g_src = aled_read_file(args->file);
    } else {
        g_src = strdup(args->oneline);
    }

    g_code = aled_parse(g_src);
    free(g_src);
    g_src = NULL;

    if (!g_code) {
        return;
    }

    if (args->fast) {
        aled_run_fast(g_code);
    } else {
        aled_run(g_code, args->debug);
    }
}

void aled_compile_call(aled_args_t *args) {
    g_src = aled_read_file(args->file);

    g_code = aled_parse(g_src);
    free(g_src);
    g_src = NULL;

    if (!g_code) {
        return;
    }

    if (args->compile == 1) {
        aled_compile(stdout, g_code);
        return;
    }
#ifdef ENABLE_BIN
    // create a temporary file to store the assembly code
    char *tmpfile = strdup("/tmp/aled-XXXXXX.s");
    mkstemp(tmpfile);

    FILE *f = fopen(tmpfile, "w");
    if (!f) {
        raise_andexit("Failed to create temporary file");
    }

    aled_compile(f, g_code);
    fclose(f);

    pid_t pid = fork();

    if (pid == 0) {
        execlp("gcc", "gcc", "-m32", "-no-pie", tmpfile, NULL);
        raise_andexit("execlp() failed");
    }

    waitpid(pid, NULL, 0);
    remove(tmpfile);
    free(tmpfile);
#else
    raise_andexit("Binary compilation is disabled");
#endif
}

void aled_start_shell(aled_args_t *args) {
    char *line = NULL;

    while (1) {
        line = aled_read_line("aled> ");
        if (!line)
            break;
        g_code = aled_parse(line);
        free(line);
        if (!g_code)
            continue;
        aled_run(g_code, args->debug);
    }
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
        raise_andexit("Cannot use file and -e options together");
    }

    if (!args.file && args.compile) {
        raise_andexit("Cannot use -c/-b without a file");
    }

    if (args.compile) {
        aled_compile_call(&args);
    } else if (!args.file && !args.oneline) {
        aled_start_shell(&args);
    } else {
        aled_execute(&args);
    }

    aled_cleanup();
    return 0;
}
