//
// Created by Bas du Pré on 05-02-18.
//

#ifndef ASSEMBLER_VM_ARCH_H
#define ASSEMBLER_VM_ARCH_H

#include <stdint.h>

enum vm_value_type_t {
    VM_TYPE_INT = 0,
    VM_TYPE_UINT,
    VM_TYPE_FLOAT,
    VM_TYPE_STRING,
    VM_TYPE_REF,    // address of a vm_value_t somewhere
    VM_TYPE_MAP, // address of a custom type (vm_obj_t)
    VM_TYPE_ARRAY,
    VM_TYPE_EMPTY,
    VM_TYPE_UNKNOWN
};

#endif //ASSEMBLER_VM_ARCH_H
