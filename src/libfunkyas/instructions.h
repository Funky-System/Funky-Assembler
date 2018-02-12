#pragma once

typedef struct {
	char* name;
	unsigned char bytecode;
	int num_operands;
} Instruction;

extern Instruction instructions[];