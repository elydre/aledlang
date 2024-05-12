#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "aledlang.h"

void aled_print_help(void) {
    puts("AledLang " ALED_VERSION
        "\nUsage: aled [options] [file]"
        "\nOptions:"
        "\n  -d   Debug, print info at each step"
        "\n  -f   Fast mode (segmentation fault on error)"
        "\n  -h   Print this help message"
        "\n  -s   Debug + step by step execution"
        "\n  -v   Print version information"
    );
}

aled_args_t aled_process_args(int argc, char **argv) {
    aled_args_t args;
    memset(&args, 0, sizeof(aled_args_t));

    if (argc < 1) {
        fprintf(stderr, "Error: no arguments specified\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            if (args.file) {
                fprintf(stderr, "Error: multiple files specified\n");
                exit(1);
            }
            args.file = argv[i];
            continue;
        }

        if (argv[i][1] == '-') {
            fprintf(stderr, "Error: invalid option -- '%s'\n", argv[i] + 2);
            exit(1);
        }

        for (int j = 1; argv[i][j]; j++) {
            switch (argv[i][j]) {
                case 'd':
                    if (!args.debug)
                        args.debug = 1;
                    break;
                case 'f':
                    args.fast = 1;
                    break;
                case 'h':
                    aled_print_help();
                    exit(0);
                case 's':
                    args.debug = 2;
                    break;
                case 'v':
                    puts("AledLang " ALED_VERSION);
                    exit(0);
                default:
                    fprintf(stderr, "Error: invalid option -- '%c'\n", argv[i][j]);
                    exit(1);
            }
        }
    }

    return args;
}
