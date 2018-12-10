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

/*
typedef struct {
	op_code_e op_code;
	reg_e rd;
	reg_e rs;
	reg_e rt;
	char imm[2];
}command_s;
*/
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
typedef enum simulator_status_e {
	SIM_SUCCESS,
	SIM_INVALID_ARGUMENT,
	SIM_IO_ERROR,
	SIM_FILE_FORMAT_ERROR
}SIMULATOR_STATUS;

simulator_context* create_simulator_context(char* memin_filename, char* memout_filename, char* regout_filename, char* trace_filename, char* count_filename);

void destroy_simulator_context(simulator_context* src);

SIMULATOR_STATUS load_memory_content(simulator_context* src, FILE* ifp);

typedef void(*inst_impl)(simulator_context* src, reg_e, reg_e, reg_e, char*, int32_t);

inst_impl get_instruction_impl(op_code_e opcode);

SIMULATOR_STATUS line_to_binary(char* line, int32_t* res);

void binary_to_line(int32_t instr, char* buffer);

SIMULATOR_STATUS write_regout(simulator_context* src);

SIMULATOR_STATUS write_memout(simulator_context* src);

SIMULATOR_STATUS update_trace(simulator_context* src);

SIMULATOR_STATUS start_simulator(simulator_context* src);

//IMM Interpretation:

uint32_t get_imm_unsigned(char* imm);

int32_t get_imm_signed(int32_t raw_inst);

//INSTRUCTION IMPLEMENTATIONS:

void add(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void sub(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void and(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void or (simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void sll(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void sra(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void limm(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void beq(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void bgt(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void ble(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void bne(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void jal(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void lw(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void sw(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void jr(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);


static const int32_t IMM_MASK = (1 << 16) - 1;
static const int32_t RT_MASK = ((1 << 20) - 1) ^ ((1 << 16) - 1);
static const int32_t RS_MASK = ((1 << 24) - 1) ^ ((1 << 16) - 1);
static const int32_t RD_MASK = ((1 << 28) - 1) ^ ((1 << 16) - 1);
static const int32_t OPCODE_MASK = (-1) ^ ((1 << 28) - 1);
static const int32_t MASK_4_BIT = (1 << 4) - 1;
static const int32_t MASK_8_BIT = (1 << 8) - 1;








