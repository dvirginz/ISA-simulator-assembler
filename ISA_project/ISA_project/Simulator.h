#pragma once
#include "reverse_assembler.h"

typedef enum simulator_status_e {
	SIM_SUCCESS,
	SIM_INVALID_ARGUMENT,
	SIM_IO_ERROR,
	SIM_FILE_FORMAT_ERROR
}SIMULATOR_STATUS;

simulator_context* create_simulator_context(char* memin_filename,char* memout_filename,char* regout_filename,char* trace_filename,char* count_filename);

void destroy_simulator_context(simulator_context* src);

SIMULATOR_STATUS load_memory_content(simulator_context* src,FILE* ifp);

typedef void(*inst_impl)(simulator_context* src,reg_e, reg_e, reg_e, char*,int32_t);

inst_impl get_instruction_impl(op_code_e opcode);

SIMULATOR_STATUS line_to_binary(char* line,int32_t* res);

void binary_to_line(int32_t instr,char* buffer);

SIMULATOR_STATUS write_regout(simulator_context* src);

SIMULATOR_STATUS write_memout(simulator_context* src);

SIMULATOR_STATUS update_trace(simulator_context* src);

SIMULATOR_STATUS start_simulator(simulator_context* src);

//IMM Interpretation:

uint32_t get_imm_unsigned(int32_t raw_inst);

int32_t get_imm_signed(char* imm);

//INSTRUCTION IMPLEMENTATIONS:

void add(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void sub(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void and(simulator_context* src,reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

void or(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst);

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











