//
// Created by Bas du Pré on 05-02-18.
//

#ifndef ASSEMBLER_ASSEMBLER_H
#define ASSEMBLER_ASSEMBLER_H

#include <stdlib.h>
#include <stdint.h>

#ifndef FUNKY_BYTECODE_TYPES_DEFINED
#define FUNKY_BYTECODE_TYPES_DEFINED
typedef unsigned char byte_t;
typedef struct funky_bytecode_t {
    byte_t* bytes;
    unsigned long length;
} funky_bytecode_t;
#endif

int funky_assemble_files(const char *filename, const char *filename_output, int strip_debug);
funky_bytecode_t funky_assemble(const char *filename_hint, const char *input, int strip_debug);

#endif //ASSEMBLER_ASSEMBLER_H
