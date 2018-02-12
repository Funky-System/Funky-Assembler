//
// Created by Bas du Pré on 05-02-18.
//

#ifndef ASSEMBLER_VM_ARCH_H
#define ASSEMBLER_VM_ARCH_H

#include <stdint.h>

#ifndef VM_ARCH_BITS
    #define VM_ARCH_BITS 32
#endif

#if VM_ARCH_BITS == 16
    typedef uint16_t vm_type_t;
    #define VM_UNSIGNED_MAX UINT16_MAX
#elif VM_ARCH_BITS == 32
    typedef uint32_t vm_type_t;
    #define VM_UNSIGNED_MAX UINT32_MAX
#elif VM_ARCH_BITS == 64
    typedef uint64_t vm_type_t;
    #define VM_UNSIGNED_MAX UINT64_MAX
#else
    #error Value for VM_ARCH_BITS is unsupported
#endif

typedef vm_type_t vm_pointer_t;

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

typedef struct {
    union {
        enum vm_value_type_t type;
        vm_type_t __padding1;
    };
    vm_type_t uint_value;
} vm_value_t;

#endif //ASSEMBLER_VM_ARCH_H
