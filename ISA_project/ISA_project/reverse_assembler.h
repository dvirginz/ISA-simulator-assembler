#pragma once
#define REG_NUM 16
#define MAX_MEM_ADDRESS 65536
#define WORD_SIZE 32
#define MAX_STRING_LEN 1024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
typedef enum {
	ADD = 0,
	SUB,
	AND,
	OR,
	SLL,
	SRA,
	LIMM,
	BEQ,
	BGT,
	BLE,
	BNE,
	JAL,
	LW,
	SW,
	JR,
	HALT,
}op_code_e;
typedef enum {
	ZERO = 0,
	AT,
	V0,
	A0,
	A1,
	T0,
	T1,
	T2,
	T3,
	S0,
	S1,
	S2,
	GP,
	SP,
	FP,
	RA,
}reg_e;

typedef struct {
	op_code_e op_code;
	reg_e rd;
	reg_e rs;
	reg_e rt;
	char imm[2];
}command_s;

typedef struct simulator_context_t {
	int32_t regs[REG_NUM]; // register content
	uint16_t PC;
	//int32_t mem[MAX_MEM_ADDRESS * WORD_SIZE]; // main memory
	int32_t mem[MAX_MEM_ADDRESS]; // main memory
	FILE* memout;
	FILE* regout;
	FILE* trace;
	FILE* count;
}simulator_context;

command_s parse_instruction(int32_t instr);


static const int32_t IMM_MASK = (1 << 16) - 1;
static const int32_t RT_MASK = ((1 << 20) - 1) ^ ((1 << 16) - 1);
static const int32_t RS_MASK = ((1 << 24) - 1) ^ ((1 << 16) - 1);
static const int32_t RD_MASK = ((1 << 28) - 1) ^ ((1 << 16) - 1);
static const int32_t OPCODE_MASK = (- 1) ^ ((1 << 28) - 1);
static const int32_t MASK_4_BIT = (1 << 4) -1;
static const int32_t MASK_8_BIT  = (1 << 8) - 1;
//command_s binary_to_command(int binary_code);














