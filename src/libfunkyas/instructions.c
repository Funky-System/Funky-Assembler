#include <stdlib.h>
#include "instructions.h"

Instruction instructions[] = {
        {.name = "export", .bytecode = 0, .num_operands = 1},
        {.name = "export.as", .bytecode = 0, .num_operands = 2},
        {.name = "data", .bytecode = 0, .num_operands = 1},

        {.name = "nop", .bytecode = 0x00, .num_operands = 0},
        {.name = "halt", .bytecode = 0x01, .num_operands = 0},
        {.name = "trap", .bytecode = 0x02, .num_operands = 1},
        {.name = "int", .bytecode = 0x03, .num_operands = 1},
        {.name = "link", .bytecode = 0x04, .num_operands = 1},
        {.name = "debug.break", .bytecode = 0x05, .num_operands = 0},
        {.name = "debug.setcontext", .bytecode = 0x06, .num_operands = 3},
        {.name = "debug.enterscope", .bytecode = 0x07, .num_operands = 1},
        {.name = "debug.leavescope", .bytecode = 0x08, .num_operands = 0},
        {.name = "unlink", .bytecode = 0x09, .num_operands = 1},
        {.name = "syscall.getindex.pop", .bytecode = 0x0B, .num_operands = 0},
        {.name = "syscall.getindex", .bytecode = 0x0C, .num_operands = 1},
        {.name = "syscall.byname", .bytecode = 0x0D, .num_operands = 1},
        {.name = "syscall", .bytecode = 0x0E, .num_operands = 1},
        {.name = "syscall.pop", .bytecode = 0x0F, .num_operands = 0},
        {.name = "link.pop", .bytecode = 0xD0, .num_operands = 0},
        {.name = "unlink.pop", .bytecode = 0xD1, .num_operands = 0},

        {.name = "ld.int", .bytecode = 0x10, .num_operands = 1},
        {.name = "ld.uint", .bytecode = 0x11, .num_operands = 1},
        {.name = "ld.float", .bytecode = 0x12, .num_operands = 1},
        {.name = "ld.str", .bytecode = 0x13, .num_operands = 1},
        {.name = "ld.map", .bytecode = 0x14, .num_operands = 0},
        {.name = "ld.local", .bytecode = 0x15, .num_operands = 1},
        {.name = "ld.reg", .bytecode = 0x16, .num_operands = 1},
        {.name = "ld.stack", .bytecode = 0x17, .num_operands = 1},
        {.name = "ld.sref", .bytecode = 0x18, .num_operands = 1},
        {.name = "ld.lref", .bytecode = 0x1A, .num_operands = 1},
        {.name = "ld.ref", .bytecode = 0x1B, .num_operands = 1},
        {.name = "ld.addr", .bytecode = 0x79, .num_operands = 1},
        {.name = "pop", .bytecode = 0x1C, .num_operands = 0},
        {.name = "st.reg", .bytecode = 0x1D, .num_operands = 1},
        {.name = "st.stack", .bytecode = 0x19, .num_operands = 1},
        {.name = "st.stack.pop", .bytecode = 0x92, .num_operands = 0},
        {.name = "st.arg.pop", .bytecode = 0x93, .num_operands = 0},
        {.name = "st.local", .bytecode = 0x1E, .num_operands = 1},
        {.name = "st.ref", .bytecode = 0x1F, .num_operands = 1},
        {.name = "st.addr", .bytecode = 0x77, .num_operands = 1},
        {.name = "swp", .bytecode = 0x78, .num_operands = 0},

        {.name = "conv.int", .bytecode = 0x20, .num_operands = 0},
        {.name = "conv.uint", .bytecode = 0x21, .num_operands = 0},
        {.name = "conv.float", .bytecode = 0x22, .num_operands = 0},
        {.name = "conv.str", .bytecode = 0x23, .num_operands = 0},
        {.name = "cast.int", .bytecode = 0x24, .num_operands = 0},
        {.name = "cast.uint", .bytecode = 0x25, .num_operands = 0},
        {.name = "cast.float", .bytecode = 0x26, .num_operands = 0},
        {.name = "cast.str", .bytecode = 0x27, .num_operands = 0},
        {.name = "cast.ref", .bytecode = 0x28, .num_operands = 0},

        {.name = "ajs", .bytecode = 0x29, .num_operands = 1},
        {.name = "locals.res", .bytecode = 0x2A, .num_operands = 1},
        {.name = "locals.cleanup", .bytecode = 0x2B, .num_operands = 0},
        {.name = "dup", .bytecode = 0x2C, .num_operands = 0},
        {.name = "deref", .bytecode = 0x2D, .num_operands = 0},
        {.name = "var", .bytecode = 0x2E, .num_operands = 0},
        {.name = "ld.deref", .bytecode = 0x2F, .num_operands = 1},

        {.name = "add", .bytecode = 0x30, .num_operands = 0},
        {.name = "sub", .bytecode = 0x31, .num_operands = 0},
        {.name = "mul", .bytecode = 0x32, .num_operands = 0},
        {.name = "div", .bytecode = 0x33, .num_operands = 0},
        {.name = "mod", .bytecode = 0x34, .num_operands = 0},
        {.name = "neg", .bytecode = 0x35, .num_operands = 0},
        {.name = "and", .bytecode = 0x36, .num_operands = 0},
        {.name = "or", .bytecode = 0x37, .num_operands = 0},
        {.name = "xor", .bytecode = 0x38, .num_operands = 0},
        {.name = "not", .bytecode = 0x39, .num_operands = 0},
        {.name = "not.bitwise", .bytecode = 0x44, .num_operands = 0},
        {.name = "cmp", .bytecode = 0x3A, .num_operands = 0},
        {.name = "eq", .bytecode = 0x3B, .num_operands = 0},
        {.name = "ne", .bytecode = 0x3C, .num_operands = 0},
        {.name = "lt", .bytecode = 0x3D, .num_operands = 0},
        {.name = "gt", .bytecode = 0x3E, .num_operands = 0},
        {.name = "le", .bytecode = 0x3F, .num_operands = 0},
        {.name = "ge", .bytecode = 0x40, .num_operands = 0},
        {.name = "pow", .bytecode = 0x41, .num_operands = 0},
        {.name = "lsh", .bytecode = 0x42, .num_operands = 0},
        {.name = "rsh", .bytecode = 0x43, .num_operands = 0},

        {.name = "beq", .bytecode = 0x50, .num_operands = 1},
        {.name = "bne", .bytecode = 0x51, .num_operands = 1},
        {.name = "blt", .bytecode = 0x52, .num_operands = 1},
        {.name = "bgt", .bytecode = 0x53, .num_operands = 1},
        {.name = "ble", .bytecode = 0x54, .num_operands = 1},
        {.name = "bge", .bytecode = 0x55, .num_operands = 1},
        {.name = "jmp", .bytecode = 0x56, .num_operands = 1},
        {.name = "brfalse", .bytecode = 0x57, .num_operands = 1},
        {.name = "brtrue", .bytecode = 0x58, .num_operands = 1},
        {.name = "call", .bytecode = 0x59, .num_operands = 2},
        {.name = "call.pop", .bytecode = 0x5A, .num_operands = 1},
        {.name = "jmp.pop", .bytecode = 0x5B, .num_operands = 0},
        {.name = "ret", .bytecode = 0x5C, .num_operands = 0},
        {.name = "args.accept", .bytecode = 0x5D, .num_operands = 1},
        {.name = "args.cleanup", .bytecode = 0x5E, .num_operands = 0},
        {.name = "ld.arg", .bytecode = 0x5F, .num_operands = 1},
        {.name = "st.arg", .bytecode = 0x7A, .num_operands = 1},

        {.name = "str.concat", .bytecode = 0x60, .num_operands = 0},
        {.name = "str.substr", .bytecode = 0x61, .num_operands = 0},
        {.name = "str.len", .bytecode = 0x62, .num_operands = 0},

        {.name = "arr.copy", .bytecode = 0x67, .num_operands = 0},
        {.name = "ld.arr", .bytecode = 0x68, .num_operands = 1},
        {.name = "ld.arrelem", .bytecode = 0x69, .num_operands = 0},
        {.name = "st.arrelem", .bytecode = 0x6A, .num_operands = 0},
        {.name = "del.arrelem", .bytecode = 0x6B, .num_operands = 0},
        {.name = "arr.len", .bytecode = 0x6C, .num_operands = 0},
        {.name = "arr.insert", .bytecode = 0x6D, .num_operands = 0},
        {.name = "arr.slice", .bytecode = 0x6E, .num_operands = 0},
        {.name = "arr.concat", .bytecode = 0x6F, .num_operands = 0},
        {.name = "conv.arr", .bytecode = 0x80, .num_operands = 0},
        {.name = "arr.range", .bytecode = 0x81, .num_operands = 0},

        {.name = "cmp.id", .bytecode = 0x70, .num_operands = 0},
        {.name = "eq.id", .bytecode = 0x71, .num_operands = 0},
        {.name = "ne.id", .bytecode = 0x72, .num_operands = 0},
        {.name = "lt.id", .bytecode = 0x73, .num_operands = 0},
        {.name = "gt.id", .bytecode = 0x74, .num_operands = 0},
        {.name = "le.id", .bytecode = 0x75, .num_operands = 0},
        {.name = "ge.id", .bytecode = 0x76, .num_operands = 0},

        {.name = "ld.extern", .bytecode = 0x90, .num_operands = 2},
        {.name = "ld.empty", .bytecode = 0x91, .num_operands = 0},

        {.name = "is.int", .bytecode = 0xA0, .num_operands = 0},
        {.name = "is.uint", .bytecode = 0xA1, .num_operands = 0},
        {.name = "is.float", .bytecode = 0xA2, .num_operands = 0},
        {.name = "is.str", .bytecode = 0xA3, .num_operands = 0},
        {.name = "is.arr", .bytecode = 0xA4, .num_operands = 0},
        {.name = "is.map", .bytecode = 0xA5, .num_operands = 0},
        {.name = "is.ref", .bytecode = 0xA6, .num_operands = 0},
        {.name = "is.empty", .bytecode = 0xA7, .num_operands = 0},

        // Object model
        {.name = "ld.mapitem", .bytecode = 0xB0, .num_operands = 1},
        {.name = "ld.mapitem.pop", .bytecode = 0xB1, .num_operands = 0},
        {.name = "st.mapitem", .bytecode = 0xB2, .num_operands = 1},
        {.name = "st.mapitem.pop", .bytecode = 0xB3, .num_operands = 0},
        {.name = "del.mapitem", .bytecode = 0xB4, .num_operands = 1},
        {.name = "del.mapitem.pop", .bytecode = 0xB5, .num_operands = 0},
        {.name = "has.mapitem", .bytecode = 0xB6, .num_operands = 1},
        {.name = "has.mapitem.pop", .bytecode = 0xB7, .num_operands = 0},
        {.name = "map.len", .bytecode = 0xB8, .num_operands = 0},
        {.name = "map.merge", .bytecode = 0xB9, .num_operands = 0},
        {.name = "map.copy", .bytecode = 0xBA, .num_operands = 0},
        {.name = "map.getprototype", .bytecode = 0xBB, .num_operands = 0},
        {.name = "map.setprototype", .bytecode = 0xBC, .num_operands = 0},
        {.name = "box", .bytecode = 0xBD, .num_operands = 0},
        {.name = "unbox", .bytecode = 0xBE, .num_operands = 0},
        {.name = "ld.boxingproto", .bytecode = 0xBF, .num_operands = 1},
        {.name = "map.renamekey", .bytecode = 0xC0, .num_operands = 2},
        {.name = "map.renamekey.pop", .bytecode = 0xC1, .num_operands = 0},
        {.name = "map.getkeys", .bytecode = 0xC2, .num_operands = 0},

        NULL
};
