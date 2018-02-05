#include <stdio.h>
#include <stdlib.h>
#include "assembler.h"

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"

int main(int argc, char **argv) {
    struct optparse_long longopts[] = {
            {"output", 'o', OPTPARSE_REQUIRED},
            {0}
    };

    int option;
    struct optparse options;
    const char *output = "out.funk";

    optparse_init(&options, argv);
    while ((option = optparse_long(&options, longopts, NULL)) != -1) {
        switch (option) {
            case 'o':
                output = options.optarg;
                break;
            case '?':
                fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
                exit(EXIT_FAILURE);
        }
    }

    if (options.optind >= argc) {
        fprintf(stderr, "%s: error: no input file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc - options.optind > 1) {
        fprintf(stderr, "%s: error: more than one input file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *filename = optparse_arg(&options);
    return assemble(filename, output);
}