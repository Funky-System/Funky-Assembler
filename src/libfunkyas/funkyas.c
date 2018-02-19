#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <funkyas/funkyas.h>

#include "instructions.h"
#include "vm_arch.h"
#include "funkyas/funkyas.h"
#include "constants.h"
#include "string_functions.h"

#define str_startswith(str, strstart) (strstr(str, strstart) == (str))

enum Section {
    SECTION_TEXT,
    SECTION_DATA,
    SECTION_EXPORTS
};

typedef struct {
    Instruction *instr;
    char *instr_str;

    char **operands_str;
    vm_type_t *operands;
    int num_operands;

    char *label;
    unsigned int offset;

    unsigned int linenum;
    const char *filename;

    enum Section section;
} Statement;

static void dereference_operands(Statement *statements, int num_statements);
static unsigned int calculate_offsets(Statement *statements, int num_statements, enum Section section, unsigned int offset);
static vm_type_t unescape(char escape, const char *filename, int linenum);
static size_t assemble_section(const Statement *statements, int num_statements, byte_t **output, size_t output_size,
                        enum Section section);
static size_t assemble_full(Statement *statements, int num_statements, byte_t **output, vm_type_t text_section_offset);

void destroy(Statement *statements, int i);

static char *strlwr(char *s) {
    char *tmp = s;

    for (; *tmp; ++tmp) {
        *tmp = (char) tolower((unsigned char) *tmp);
    }

    return s;
}

static int isstralphanum(char *s) {
    for (; *s; ++s) {
        if (!isalnum(*s) && *s != '_' && *s != '.' && *s != '@') return 0;
    }
    return 1;
}

static Instruction *find_instr(const char *instr_str) {
    unsigned int i = 0;
    Instruction *instr;
    do {
        instr = &instructions[i++];
    } while (instr->name != NULL && strcmp(instr->name, instr_str) != 0);

    return instr->name == NULL ? NULL : instr;
}

int ends_in_open_string_literal(char *str) {
    size_t len = strlen(str);
    int open = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '\\') {
            i++;
        } else {
            if (str[i] == '"') {
                open = !open;
            }
        }
    }
    return open;
}

int funky_assemble_files(const char *filename_in, const char *filename_output, int strip_debug) {
    FILE *fp;
    fp = fopen(filename_in, "r");
    if (fp == NULL) {
        int errnum = errno;
        fprintf(stderr, "Error: Could not open file %s\n", filename_in);
        fprintf(stderr, "%s", strerror(errnum));
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0L, SEEK_END);
    size_t numbytes = (size_t)ftell(fp);

    // reset the file position indicator to the beginning of the file
    fseek(fp, 0L, SEEK_SET);

    char *code_in = malloc(numbytes + 1);
    fread(code_in, sizeof(char), numbytes, fp);
    code_in[numbytes] = '\0';

    fclose(fp);

    funky_bytecode_t output = funky_assemble(filename_in, code_in, strip_debug);
    free(code_in);

    FILE *outFile = fopen(filename_output, "wb");
    if (outFile == NULL) {
        int errnum = errno;
        fprintf(stderr, "Error: Could not write to file %s\n", filename_output);
        fprintf(stderr, "%s\n", strerror(errnum));
        exit(EXIT_FAILURE);
    }
    fwrite(output.bytes, sizeof(char), output.length, outFile);
    free(output.bytes);

    fclose(outFile);

    return 1;
}

funky_bytecode_t funky_assemble(const char *filename_hint, const char *input, int strip_debug) {
    unsigned int linenum = 0;

    Statement *statements = NULL;
    int num_statements = 0;

    enum Section section = SECTION_TEXT;

    int first_line = 1;

    while ((input = first_line ? input : strchr(input, '\n') + 1) != NULL + 1) {
        first_line = 0;

        const char *next_line = strchr(input, '\n');
        size_t length;
        if (next_line != NULL) {
            length = next_line - input + 1;
        } else {
            length = strlen(input);
        }
        char *orig_line = malloc(length + 1);
        orig_line[0] = '\0';
        strncat(orig_line, input, length);

        char *line = orig_line;
        linenum++;

        // Find comment and remove everything past the comment char (#)
        char *comment = strpbrk(line, "#");
        while (comment != NULL) {
            *comment = '\0';
            if (ends_in_open_string_literal(line)) {
                *comment = '#';
                comment = strpbrk(comment + 1, "#");
            } else {
                break;
            }
        }

        // Trim whitespace from front
        while (line[0] == '\t' || line[0] == ' ') {
            line++;
        }

        // Check for section
        if (str_startswith(line, "section ")) {
            char section_str[128];
            section_str[0] = '\0';
            unsigned long len = strpbrk(line + 8, " \n\t") - (line + 8);
            strncat(section_str, line + 8, len);
            strlwr(section_str);

            if (strcmp(section_str, ".data") == 0) section = SECTION_DATA;
            else if (strcmp(section_str, ".text") == 0) section = SECTION_TEXT;
            else if (strcmp(section_str, ".exports") == 0) section = SECTION_EXPORTS;
            else {
                fprintf(stderr, "%s:%d Error: %s is not a valid section title\n", filename_hint, linenum, section_str);
                exit(EXIT_FAILURE);
            }
            continue;
        }

        num_statements++;
        statements = realloc(statements, sizeof(Statement) * num_statements);
        memset(&statements[num_statements - 1], 0, sizeof(Statement));
        Statement *statement = &statements[num_statements - 1];
        statement->linenum = linenum;
        statement->filename = filename_hint;
        statement->section = section;

        // Check for label
        char *label = strpbrk(line, ":");
        if (label != NULL) {
            *label = '\0';
            if (!ends_in_open_string_literal(line)) {
                statement->label = malloc(strlen(line) + 1);
                strcpy(statement->label, line);
                while (statement->label[0] == '\t' || statement->label[0] == ' ') {
                    statement->label++;
                }
                while (statement->label[strlen(statement->label) - 1] == '\t' ||
                       statement->label[strlen(statement->label) - 1] == ' ') {
                    statement->label[strlen(statement->label) - 1] = '\0';
                }
                if (label[1] != '\0') {
                    line = label + 1;
                } else {
                    line = label;
                }
            } else {
                *label = ':';
            }
        }

        // Get the instruction
        char *instr = strtok(line, " \t\n");

        if (instr != NULL) {
            strlwr(instr);

            if (strip_debug && str_startswith(instr, "debug.")) {
                continue;
            }

            statement->instr_str = malloc(strlen(instr) + 1);
            strcpy(statement->instr_str, instr);

            statement->instr = find_instr(instr);
            if (statement->instr == NULL) {
                fprintf(stderr, "%s:%d: Instruction '%s' is not valid\n", filename_hint, linenum, instr);
                exit(EXIT_FAILURE);
            }

            if (strcmp(instr, "export") == 0 || strcmp(instr, "export.as") == 0) {
                if (section != SECTION_EXPORTS) {
                    fprintf(stderr, "%s:%d EXPORT found in section other than .EXPORTS'\n", filename_hint, linenum);
                    exit(EXIT_FAILURE);
                }
            }

            char *remainder = strtok(NULL, "");

            char *operand_tok = strsep(&remainder, ",\n");

            while (operand_tok != NULL) {
                if (strcmp(operand_tok, "") != 0) {
                    while (ends_in_open_string_literal(operand_tok)) {
                        if (remainder == NULL) {
                            fprintf(stderr, "%s:%d Unterminated string literal found\n", filename_hint, linenum);
                            exit(EXIT_FAILURE);
                        }
                        operand_tok[strlen(operand_tok)] = ',';
                        strsep(&remainder, ",\n");
                    }

                    statement->num_operands++;
                    statement->operands_str = realloc(statement->operands_str,
                                                      sizeof(char *) * statement->num_operands);
                    char *operand = malloc(strlen(operand_tok) + 1);
                    char *orig_operand = operand; // to pass to free
                    strcpy(operand, operand_tok);
                    while (operand[0] == '\t' || operand[0] == ' ')
                        operand++;
                    operand = strdup(operand);
                    free(orig_operand);

                    while (operand[strlen(operand) - 1] == '\t' || operand[strlen(operand) - 1] == ' ' ||
                           operand[strlen(operand) - 1] == '\n')
                        operand[strlen(operand) - 1] = '\0';

                    if (operand[0] == '"') {
                        char *replaced_data = fas_str_replace(operand, "\\\"", "\"");
                        fas_str_replace_inplace(&replaced_data, "\\n", "\n");
                        fas_str_replace_inplace(&replaced_data, "\\0", "\0");
                        free(operand);
                        operand = replaced_data;
                    }

                    statement->operands_str[statement->num_operands - 1] = operand;

                    if ((strcmp(instr, "data") != 0 && strcmp(instr, "export.as") != 0) && operand[0] == '"') {
                        // this is a string, create a .data statement
                        char *data_label = malloc(12);
                        int num_operands = statement->num_operands;
                        sprintf(data_label, "__str_%d_%d", linenum, statement->num_operands);
                        num_statements++;
                        statements = realloc(statements, sizeof(Statement) * num_statements);
                        memset(&statements[num_statements - 1], 0, sizeof(Statement));
                        statement = &statements[num_statements - num_operands -
                                                1]; // realloc might have changed it's position, so reset it
                        Statement *data_statement = &statements[num_statements - 1];
                        data_statement->linenum = linenum;
                        data_statement->filename = filename_hint;
                        data_statement->section = SECTION_DATA;
                        data_statement->instr = find_instr("data");
                        data_statement->instr_str = strdup("data");
                        data_statement->label = data_label;
                        data_statement->operands_str = malloc(sizeof(char *));
                        data_statement->operands_str[0] = statement->operands_str[statement->num_operands - 1];
                        data_statement->num_operands = 1;

                        statement->operands_str[statement->num_operands - 1] = strdup(data_label);
                    }
                }
                operand_tok = strsep(&remainder, ",\n");
            }
            if (statement->num_operands != statement->instr->num_operands) {
                fprintf(stderr, "%s:%d Instruction '%s' expects %d operand(s). Currently given: %d\n", filename_hint, linenum,
                       instr, statement->instr->num_operands, statement->num_operands);
                exit(EXIT_FAILURE);
            }
        }
        free(orig_line);
    }

    unsigned int offset = 0;
    offset = calculate_offsets(statements, num_statements, SECTION_EXPORTS, offset);
    offset = calculate_offsets(statements, num_statements, SECTION_DATA, offset);
    vm_type_t text_section_offset = offset;
    offset = calculate_offsets(statements, num_statements, SECTION_TEXT, offset);

    dereference_operands(statements, num_statements);

    byte_t *output = malloc(1);
    size_t size_out = assemble_full(statements, num_statements, &output, text_section_offset);
    destroy(statements, num_statements);

    return (funky_bytecode_t) {
            .bytes = output,
            .length = size_out
    };
}

void destroy(Statement *statements, int num) {
    for (int i = 0; i < num; i++) {
        free(statements[i].instr_str);
        if (statements[i].label != NULL) free(statements[i].label);
        for (int j = 0; j < statements[i].num_operands; j++) {
            free(statements[i].operands_str[j]);
        }
        free(statements[i].operands_str);
        free(statements[i].operands);
    }
    free(statements);
}

static unsigned int calculate_offsets(Statement *statements, int num_statements, enum Section section, unsigned int offset) {
    for (int i = 0; i < num_statements; i++) {
        if (statements[i].section != section) continue;

        statements[i].offset = offset;
        if (statements[i].instr == NULL) continue;

        if (strcmp(statements[i].instr->name, "var") == 0) {
            offset += sizeof(vm_value_t);
        } else if (strcmp(statements[i].instr->name, "data") == 0) {
            if (statements[i].operands_str[0][0] == '"') {
                offset += sizeof(vm_type_t) + strlen(statements[i].operands_str[0]) - 2 + 1;
            } else if (statements[i].operands_str[0][0] == '\'') {
                offset += 1;
            } else {
                offset += sizeof(vm_type_t);
            }
        } else if (strcmp(statements[i].instr->name, "export") == 0) {
            offset += strlen(statements[i].operands_str[0]) + 1;
            offset += sizeof(vm_type_t);
        } else if (strcmp(statements[i].instr->name, "export.as") == 0) {
            offset += strlen(statements[i].operands_str[1]) - 2 + 1;
            offset += sizeof(vm_type_t);
        } else {
            offset += 1; // bytecode
            offset += statements[i].instr->num_operands * sizeof(uint32_t);
        }
    }

    return offset;
}

static void dereference_operands(Statement *statements, int num_statements) {
    for (int i = 0; i < num_statements; i++) {
        statements[i].operands = malloc(sizeof(vm_type_t) * statements[i].num_operands);
        for (int o = 0; o < statements[i].num_operands; o++) {
            char *operand_str = statements[i].operands_str[o];

            char *pEnd;
            // int literal
            long literal = strtol(operand_str, &pEnd, 0);
            if (literal > UINT32_MAX || literal < INT32_MIN) {
                fprintf(stderr, "%s:%d Warning: literal '%s' overflows 32 bits\n", statements[i].filename, statements[i].linenum,
                       operand_str);
            }

            if (*pEnd == '\0') {
                // whole string has been converted to an integer
                statements[i].operands[o] = (vm_type_t) literal;
                continue;
            }

            // float literal
            float fliteral = strtof(operand_str, &pEnd);
            if (*pEnd == '\0') {
                // whole string has been converted to an float
                memcpy(&(statements[i].operands[o]), &fliteral, sizeof(fliteral));
                continue;
            }

            if (operand_str[0] == '%') {
                // reference a register
                char *reg_str = operand_str + 1;
                strlwr(reg_str);
                if (strcmp(reg_str, "pc") == 0) {
                    statements[i].operands[o] = 0;
                } else if (strcmp(reg_str, "sp") == 0) {
                    statements[i].operands[o] = 1;
                } else if (strcmp(reg_str, "mp") == 0) {
                    statements[i].operands[o] = 2;
                } else if (strcmp(reg_str, "ap") == 0) {
                    statements[i].operands[o] = 3;
                } else if (strcmp(reg_str, "rr") == 0) {
                    statements[i].operands[o] = 4;
                } else if (strcmp(reg_str, "r0") == 0) {
                    statements[i].operands[o] = 5;
                } else if (strcmp(reg_str, "r1") == 0) {
                    statements[i].operands[o] = 6;
                } else if (strcmp(reg_str, "r2") == 0) {
                    statements[i].operands[o] = 7;
                } else if (strcmp(reg_str, "r3") == 0) {
                    statements[i].operands[o] = 8;
                } else if (strcmp(reg_str, "r4") == 0) {
                    statements[i].operands[o] = 9;
                } else if (strcmp(reg_str, "r5") == 0) {
                    statements[i].operands[o] = 10;
                } else if (strcmp(reg_str, "r6") == 0) {
                    statements[i].operands[o] = 11;
                } else if (strcmp(reg_str, "r7") == 0) {
                    statements[i].operands[o] = 12;
                } else {
                    fprintf(stderr, "%s:%d '%s' is not a valid register\n", statements[i].filename, statements[i].linenum,
                           operand_str);
                    exit(EXIT_FAILURE);
                }
                continue;
            }

            if (isstralphanum(operand_str)) {
                // check for constant
                if (is_constant(operand_str)) {
                    statements[i].operands[o] = (vm_type_t) get_constant(operand_str);
                    continue;
                } else {
                    // looks like a label
                    int found_label = 0;
                    for (int j = 0; j < num_statements; j++) {
                        if (statements[j].label != NULL && strcmp(operand_str, statements[j].label) == 0) {
                            // found a label!
                            statements[i].operands[o] = statements[j].offset;
                            found_label = 1;
                            break;
                        }
                    }
                    if (found_label) continue;

                    fprintf(stderr, "%s:%d Could not find label '%s'\n", statements[i].filename, statements[i].linenum,
                           operand_str);
                    exit(EXIT_FAILURE);
                }
            }

            if (operand_str[0] == '\'') {
                // char
                if (strlen(operand_str) != 3 || operand_str[2] != '\'') {
                    if (operand_str[1] == '\\') {
                        statements[i].operands[o] = unescape(operand_str[2], statements[i].filename,
                                                             statements[i].linenum);
                    } else {
                        fprintf(stderr, "%s:%d Operand starts with a quote ('), but is not a valid char literal: '%s'\n",
                               statements[i].filename, statements[i].linenum, operand_str);
                        exit(EXIT_FAILURE);
                    }
                } else {
                    statements[i].operands[o] = (vm_type_t) operand_str[1];
                }
                continue;
            }

            if (operand_str[0] == '\"') {
                // string
                //if (strcmp(statements[i].instr->name, "data") == 0) {
                // do nothing, the string is saved in operand_str
                //} else {

                //}
                continue;
            }

            // if we are heren, I don't know what to do with this operand...
            fprintf(stderr, "%s:%d Unexpected operand specified: '%s'\n", statements[i].filename, statements[i].linenum,
                   operand_str);
            exit(EXIT_FAILURE);
        }
    }
}

static vm_type_t unescape(char escape, const char *filename, int linenum) {
    switch (escape) {
        case '\'':
            return '\'';
        case '"':
            return '"';
        case '?':
            return '\?';
        case '\\':
            return '\\';
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 't':
            return '\t';
        case 'v':
            return '\v';
        case '0':
            return '\0';
        case '1':
            return '\1';
        case '2':
            return '\2';
        case '3':
            return '\3';
        case '4':
            return '\4';
        case '5':
            return '\5';
        case '6':
            return '\6';
        case '7':
            return '\7';
        default:
            fprintf(stderr, "%s:%d \\%c is not a valid escape sequence\n",
                   filename, linenum, escape);
            exit(EXIT_FAILURE);
    }
}

#define IS_BIG_ENDIAN (!*(unsigned char *)&(uint16_t){1})

#define FLAG_LITTLE_ENDIAN 1
//#define FLAG_LITTLE_ENDIAN 2
//#define FLAG_LITTLE_ENDIAN 4
//#define FLAG_LITTLE_ENDIAN 8
//#define FLAG_LITTLE_ENDIAN 16
//#define FLAG_LITTLE_ENDIAN 32
//#define FLAG_LITTLE_ENDIAN 64
//#define FLAG_LITTLE_ENDIAN 128

static vm_type_t get_num_exports(Statement *statements, int num_statements) {
    vm_type_t num = 0;
    for (int i = 0; i < num_statements; i++) {
        if (statements[i].instr != NULL && str_startswith(statements[i].instr->name, "export"))
            num++;
    }
    return num;
}

static size_t assemble_full(Statement *statements, int num_statements, byte_t **output, vm_type_t text_section_offset) {
    *output = realloc(*output, 0);
    size_t output_size = 0;

    // header
    output_size += 6 + 2 * sizeof(vm_type_t);
    *output = realloc(*output, output_size);
    memcpy(*output, "funk", 4);

    byte_t flags = 0;

    if (!IS_BIG_ENDIAN) {
        flags |= FLAG_LITTLE_ENDIAN;
    }

    (*output)[4] = flags;
    (*output)[5] = sizeof(vm_type_t);
    *((vm_type_t *) ((*output) + 6)) = get_num_exports(statements, num_statements);
    *((vm_type_t *) ((*output) + 6 + sizeof(vm_type_t))) = text_section_offset;

    // code
    output_size = assemble_section(statements, num_statements, output, output_size, SECTION_EXPORTS);
    output_size = assemble_section(statements, num_statements, output, output_size, SECTION_DATA);
    output_size = assemble_section(statements, num_statements, output, output_size, SECTION_TEXT);

    return output_size;

}

static size_t assemble_section(const Statement *statements, int num_statements, byte_t **output, size_t output_size,
                        enum Section section) {
    for (int i = 0; i < num_statements; i++) {
        if (statements[i].section != section) continue;

        size_t offset = output_size;
        if (statements[i].instr == NULL) continue;

        if (strcmp(statements[i].instr->name, "var") == 0) {
            output_size += sizeof(vm_value_t);
            *output = realloc(*output, output_size);
            for (int b = 0; b < sizeof(vm_value_t); b++) {
                (*output)[offset++] = 0;
            }
        } else if (strcmp(statements[i].instr->name, "data") == 0) {
            if (statements[i].operands_str[0][0] == '"') {
                output_size += sizeof(vm_type_t) + strlen(statements[i].operands_str[0]) - 2 + 1;
                *output = realloc(*output, output_size);
                *(vm_type_t *) ((*output) + offset) = VM_UNSIGNED_MAX;
                offset += sizeof(vm_type_t);
                (*output)[offset] = '\0';
                strncat((char*)((*output) + offset), statements[i].operands_str[0] + 1,
                        strlen(statements[i].operands_str[0]) - 2);
            } else if (statements[i].operands_str[0][0] == '\'') {
                output_size += 1;
                *output = realloc(*output, output_size);
                (*output)[offset] = (char) statements[i].operands[0];
            } else {
                output_size += sizeof(vm_type_t);
                *output = realloc(*output, output_size);
                *(vm_type_t *) ((*output) + offset) = statements[i].operands[0];
            }
        } else if (strcmp(statements[i].instr->name, "export") == 0) {
            output_size += strlen(statements[i].operands_str[0]) + 1 + sizeof(vm_type_t);
            *output = realloc(*output, output_size);
            strcpy((char*)((*output) + offset), statements[i].operands_str[0]);
            offset += strlen(statements[i].operands_str[0]) + 1;
            for (int b = 0; b < sizeof(vm_type_t); b++) {
                (*output)[offset++] = ((byte_t *) (&statements[i].operands[0]))[b];
            }
        } else if (strcmp(statements[i].instr->name, "export.as") == 0) {
            char *name = malloc(strlen(statements[i].operands_str[1]) - 1);
            name[0] = '\0';
            strncat(name, statements[i].operands_str[1] + 1, strlen(statements[i].operands_str[1]) - 2);

            output_size += strlen(name) + 1 + sizeof(vm_type_t);
            *output = realloc(*output, output_size);
            strcpy((char*)((*output) + offset), name);
            offset += strlen(name) + 1;
            for (int b = 0; b < sizeof(vm_type_t); b++) {
                (*output)[offset++] = ((byte_t *) (&statements[i].operands[0]))[b];
            }
        } else {
            output_size += 1 + statements[i].num_operands * sizeof(vm_type_t);
            *output = realloc(*output, output_size);
            (*output)[offset++] = statements[i].instr->bytecode;
            for (int o = 0; o < statements[i].num_operands; o++) {
                for (int b = 0; b < sizeof(vm_type_t); b++) {
                    (*output)[offset++] = ((byte_t *) (&statements[i].operands[o]))[b];
                }
            }
        }
    }

    return output_size;
}
