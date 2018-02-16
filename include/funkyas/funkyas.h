//
// Created by Bas du Pré on 05-02-18.
//

#ifndef ASSEMBLER_ASSEMBLER_H
#define ASSEMBLER_ASSEMBLER_H

int assemble_file_to_file(const char *filename, const char *filename_output, int strip_debug);
char* assemble_string_to_string(const char *filename_hint, const char *input, int strip_debug, size_t *size_out);

#endif //ASSEMBLER_ASSEMBLER_H
