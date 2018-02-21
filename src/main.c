#include <stdio.h>
#include <stdlib.h>
#include "funkyas/funkyas.h"

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"
#include "version.h"
#include "libfunkyas/vm_arch.h"

#define IS_BIG_ENDIAN (!*(unsigned char *)&(uint16_t){1})

int main(int argc, char **argv) {
    struct optparse_long longopts[] = {
            {"output", 'o', OPTPARSE_REQUIRED},
            {"strip-debug", 'r', OPTPARSE_REQUIRED},
            {"version", 'v', OPTPARSE_NONE},
            {0}
    };

    int option;
    struct optparse options;
    int strip_debug = 0;
    const char *output = "out.funk";

    optparse_init(&options, argv);
    while ((option = optparse_long(&options, longopts, NULL)) != -1) {
        switch (option) {
            case 'o':
                output = options.optarg;
                break;
            case 'r':
                strip_debug = 1;
                break;
            case 'v':
                printf("Funky Assembler version %s.%s.%s\nTarget: %d bits (%s)\nBuilt on %s %s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION,
                       (int)sizeof(vm_type_t) * 8, IS_BIG_ENDIAN ? "big-endian" : "little-endian",
                       __DATE__, __TIME__);
                return 0;
            default:
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
    if (funky_assemble_files(filename, output, strip_debug)) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}