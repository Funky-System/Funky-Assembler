//
// Created by Bas du Pré on 05-02-18.
//

#ifndef ASSEMBLER_ASSEMBLER_H
#define ASSEMBLER_ASSEMBLER_H

#include <stdlib.h>
#include <stdint.h>

#include "vm_arch.h"

typedef unsigned char byte_t;
typedef struct {
    byte_t* bytes;
    vm_type_t length;
} funky_bytecode_t;

int funky_assemble_files(const char *filename, const char *filename_output, int strip_debug);
funky_bytecode_t funky_assemble(const char *filename_hint, const char *input, int strip_debug);

#endif //ASSEMBLER_ASSEMBLER_H
