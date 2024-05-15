#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "aledlang.h"

void aled_print_help(void) {
    puts("Usage: aled [options] [file]"
        "\nOptions:"
#ifdef ENABLE_BIN
        "\n  -b   Compile to elf i386 (requires gcc)"
#endif
        "\n  -c   Compile to gnu asm i386"
        "\n  -e   Execute code from next argument"
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
        raise_andexit("No arguments specified");
    }

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            if (args.file)
                raise_andexit("Multiple files specified");
            args.file = argv[i];
            continue;
        }

        if (argv[i][1] == '-') {
            raise_andexit("Invalid option -- '%s'", argv[i] + 2);
        }

        if (strcmp(argv[i], "-e") == 0) {
            if (args.oneline)
                raise_andexit("Multiple -c options specified");
            if (i + 1 == argc)
                raise_andexit("No code specified for -c");
            args.oneline = argv[++i];
            continue;
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
                case 'c':
                    args.compile |= 1;
                    break;
#ifdef ENABLE_BIN
                case 'b':
                    args.compile |= 2;
                    break;
#endif
                case 'h':
                    aled_print_help();
                    aled_cleanup();
                    exit(0);
                case 's':
                    args.debug = 2;
                    break;
                case 'v':
                    puts("AledLang " ALED_VERSION);
                    aled_cleanup();
                    exit(0);
                default:
                    raise_andexit("Invalid option -- '%c'", argv[i][j]);
            }
        }
    }

    return args;
}
