#include <string.h>
#include "constants.h"

struct constant {
    const char *str;
    int val;
};

static struct constant constants[] = {
        {"VM_TYPE_INT", VM_TYPE_INT},
        {"VM_TYPE_UINT", VM_TYPE_UINT},
        {"VM_TYPE_FLOAT", VM_TYPE_FLOAT},
        {"VM_TYPE_STRING", VM_TYPE_STRING},
        {"VM_TYPE_ARRAY", VM_TYPE_ARRAY},
        {"VM_TYPE_MAP", VM_TYPE_MAP},
        {0}
};

int is_constant(const char* str) {
    int i = 0;
    while (constants[i].str != 0) {
        if (strcmp(str, constants[i].str) == 0) return 1;
        i++;
    }
    return 0;
}

int get_constant(const char* str) {
    int i = 0;
    while (constants[i].str != 0) {
        if (strcmp(str, constants[i].str) == 0) return constants[i].val;
        i++;
    }
    return 0;
}
