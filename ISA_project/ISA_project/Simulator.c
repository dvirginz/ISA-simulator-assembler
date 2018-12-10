#include "Simulator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#pragma warning(disable : 4996)
#define WORD_HEX_SIZE 8
//#define REG_NUM 16
simulator_context* create_simulator_context(char* memin_filename, char* memout_filename, char* regout_filename, char* trace_filename, char* count_filename){
	if (!memin_filename || !memout_filename || !regout_filename || !trace_filename || !count_filename) {
		return NULL;
	}
	simulator_context* res = (simulator_context*)calloc(1,sizeof(simulator_context));
	FILE* memin = fopen(memin_filename, "r");
	fprintf(stderr, "MEMIN: %s\n", strerror(errno));
	FILE* memout = fopen(memout_filename, "w");
	fprintf(stderr, "MEMOUT: %s\n", strerror(errno));
	FILE* regout = fopen(regout_filename, "w");
	fprintf(stderr, "REGOUT: %s\n", strerror(errno));
	FILE* trace = fopen(trace_filename, "w");
	fprintf(stderr, "TRACE: %s\n", strerror(errno));
	FILE* count = fopen(count_filename, "w");
	fprintf(stderr, "COUNT: %s\n", strerror(errno));
	if (!res || !memin || !memout || !regout || !trace || !count) {
		fprintf(stderr, "COULD NOT OPEN FILE:\n %s\n", strerror(errno));
		if (memin) { fclose(memin); }
		if (memout) { fclose(memout); }
		if (regout) { fclose(regout); }
		if (trace) { fclose(trace); }
		if (count) { fclose(count); }
		if (res) { free(res); }
		return NULL;
	}
	res->memout = memout;
	res->regout = regout;
	res->trace = trace;
	res->count = count;
	//if we failed to read memory content, this will clean everything up
	if (load_memory_content(res, memin) != SIM_SUCCESS) {
		destroy_simulator_context(res);
		res = NULL;
	}
	fclose(memin);
	return res;
}

void destroy_simulator_context(simulator_context* src) {
	if (src) {
		fclose(src->memout);
		fclose(src->regout);
		fclose(src->trace);
		fclose(src->count);
		free(src);
	}
}

SIMULATOR_STATUS load_memory_content(simulator_context* src,FILE* ifp) {
	if (!src || !ifp) { return SIM_INVALID_ARGUMENT; }
	char buffer[MAX_STRING_LEN + 1];
	buffer[MAX_STRING_LEN] = '\0';
	size_t len = 0; //dummy variable for getline
	int curr_mem_pos = 0; //location in memory
	int32_t mem_line;
	while (fgets(buffer, MAX_STRING_LEN, ifp) != NULL && curr_mem_pos < MAX_MEM_ADDRESS) {
		if (line_to_binary(buffer, &mem_line) != SIM_SUCCESS) {
			return SIM_FILE_FORMAT_ERROR;
		}
		src->mem[curr_mem_pos] = mem_line;
		memset(buffer, MAX_STRING_LEN , sizeof(char));
		curr_mem_pos++;
	}
	return SIM_SUCCESS;
}

SIMULATOR_STATUS line_to_binary(char* line,int32_t* res) {
	if (!line) {return SIM_INVALID_ARGUMENT;}
	int count = 0; //number of Zeor's/One's found.
	int32_t res_out = 0; 
	size_t line_length = strlen(line);
	for (int i = 0; i < line_length && count < WORD_SIZE ; i++) {
		char c[2]; //represent the digit as string
		c[0] = line[i];
		c[1] = '\0';
		if (isxdigit(c[0])){ //we've found a relevant char.
			res_out <<= 4; // shift left by 4 for the next 4 bits.
			res_out += (int32_t)strtol(c,NULL,16); //set the last 4 bits bit to relevant value
			//res_out <<= 4; // shift left by 4 for the next 4 bits.
			count += 4;
		}
	}
	*res = res_out;
	if (count != WORD_SIZE) { return SIM_FILE_FORMAT_ERROR; }
	return SIM_SUCCESS;
}

inst_impl get_instruction_impl(op_code_e opcode) {
	static inst_impl impls[] = { &add, &sub,&and,&or ,&sll,&sra,&limm,&beq,&bgt,&ble,&bne,&jal,&lw,&sw,&jr,NULL };
	return impls[opcode];
}

int32_t get_imm_signed(int32_t raw_inst) {
	int32_t immediate = (1 << 16) - 1;
	immediate &= raw_inst;
	immediate <<= 16;
	immediate >>= 16;
	return immediate;
}

uint32_t get_imm_unsigned(char* imm) {
	uint16_t imm16 = 0;
	imm16 |= (int16_t)(imm[0]);
	imm16 <<= 8;
	imm16 |= (int16_t)(imm[1]);
	return (uint32_t)(imm16);
}

void add(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src || rd == ZERO) {
		src->PC++;
		fprintf(stderr, "ADD - TARGET WAS $ZERO, NOTHING DONE. PC IS: %d RD: %d \n",src->PC -1,rd);
		return; 
	} //no action if destination is zero.
	fprintf(stderr, "ADD - SETTING %d to %d + %d\n", rd, src->regs[rs], src->regs[rt]);
	src->regs[rd] = src->regs[rs] + src->regs[rt];
	
	src->PC++;
}

void sub(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src || rd == ZERO) { 
		fprintf(stderr, "SUB - TARGET WAS $ZERO, NOTHING DONE\n");
		src->PC++;
		return; 
	} //no action if destination is zero.
	fprintf(stderr, "SUB - SETTING %d to %d - %d\n", rd, src->regs[rs], src->regs[rt]);
	src->regs[rd] = src->regs[rs] - src->regs[rt];
	src->PC++;
}

void and(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src || rd == ZERO) {
		fprintf(stderr, "AND - TARGET WAS $ZERO, NOTHING DONE\n");
		src->PC++;
		return; 
	} //no action if destination is zero.
	fprintf(stderr, "AND - SETTING %d to %d & %d, result = %d \n",rd, src->regs[rs] , src->regs[rt],src->regs[rs] & src->regs[rt]);
	src->regs[rd] = src->regs[rs] & src->regs[rt];
	src->PC++;
}

void or(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src || rd == ZERO) {
		fprintf(stderr, "OR - TARGET WAS $ZERO, NOTHING DONE\n");
		src->PC++;
		return; 
	} //no action if destination is zero.
	fprintf(stderr, "OR - SETTING %d to %d | %d, result = %d \n", rd, src->regs[rs], src->regs[rt], src->regs[rs] | src->regs[rt]);
	src->regs[rd] = src->regs[rs] | src->regs[rt];
	src->PC++;
}

void sll(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src || rd == ZERO) {
		src->PC++;
		fprintf(stderr, "SLL - TARGET WAS $ZERO, NOTHING DONE\n");
		return; 
	} //no action if destination is zero.
	fprintf(stderr, "SLL %d %d %d \n",rd,rs,rt);
	src->regs[rd] = src->regs[rs] << src->regs[rt];
	src->PC++;
}

void sra(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src || rd == ZERO) { 
		fprintf(stderr, "SRA - TARGET WAS $ZERO, NOTHING DONE\n");
		src->PC++;
		return; 
	} //no action if destination is zero.
	fprintf(stderr, "SRA %d %d %d \n", rd, rs, rt);
	src->regs[rd] = src->regs[rs] >> src->regs[rt];// these are signed values, in VS2015 this is an arithmetic shift.
	src->PC++;
}

void limm(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src || rd == ZERO) {
		fprintf(stderr, "LIMM - TARGET WAS $ZERO, NOTHING DONE\n");
		src->PC++;
		return;
	} //no action if destination is zero.
	src->regs[rd] = get_imm_signed(raw_inst);
	fprintf(stderr,"LIMM: setting %d to %d\n", rd, get_imm_signed(raw_inst));
	src->PC++;
}

void beq(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src ) {
		return;
	} 
	if(src->regs[rs] == src->regs[rt]) {
		src->PC = get_imm_unsigned(imm);
		fprintf(stderr, "BEQ - BRANCH TAKEN, NEW PC IS %d\n",src->PC);
		return;
	}
	fprintf(stderr, "BEQ - BRANCH NOT TAKEN. PC IS:\n", src->PC);
	src->PC++;
}

void bgt(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src) {
		return;
	} 
	if(src->regs[rs] > src->regs[rt]) {
		fprintf(stderr, "BGT - BRANCH TAKEN, NEW PC IS %d\n", src->PC);
		src->PC = get_imm_unsigned(imm);
		return;
	}
	fprintf(stderr, "BGT - BRANCH NOT TAKEN. PC IS %d\n", src->PC);
	src->PC++;
}

void ble(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src) {
		return;
	} 
	if(src->regs[rs] <= src->regs[rt]) {
		fprintf(stderr, "BLE - BRANCH TAKEN, NEW PC IS %d\n", src->PC);
		src->PC = get_imm_unsigned(imm);
		return;
	}
	src->PC++;
	fprintf(stderr, "BLE - BRANCH NOT TAKEN. PC IS %d\n", src->PC);

}

void bne(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src) {
		return;
	} 
	if(src->regs[rs] != src->regs[rt] ) {
		fprintf(stderr, "BNE - BRANCH TAKEN, NEW PC IS %d\n", src->PC);
		src->PC = get_imm_unsigned(imm);
		return;
	}
	fprintf(stderr, "BNE - BRANCH NOT TAKEN. PC IS %d\n", src->PC);
	src->PC++;
}

void jal(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src) {
		return;
	} 
	src->regs[15] = src->PC + 1;
	src->PC = get_imm_unsigned(imm);
	fprintf(stderr, "JAL - NEW PC IS: %d\n", src->PC);
}

//TODO: VERIFY THIS IS CORRECT IN TERMS OF SIGNS
void lw(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src || rd == ZERO) {
		fprintf(stderr, "LW - TARGET WAS $ZERO, NOTHING DONE\n");
		src->PC++;
		return;
	} //no action if destination is zero.
	uint16_t addr = (uint16_t)(src->regs[rs] + get_imm_signed(raw_inst));
	fprintf(stderr, "ADDR: %d \n", addr);
	src->regs[rd] = src->mem[addr];
	fprintf(stderr, "LW - LOADED WORD FROM BASE ADDRESS %d offset %d to %d, value:%d\n", src->regs[rs], get_imm_signed(raw_inst), rd, src->mem[addr]);
	src->PC++;
}

void sw(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src ) {
		return;
	} 
	uint16_t addr = (uint16_t)(src->regs[rs] + get_imm_signed(raw_inst));
	fprintf(stderr, "ADDR: %d \n", addr);
	src->mem[addr] = src->regs[rd];
	fprintf(stderr, "SW - STORED  WORD TO BASE ADDRESS %d offset %d FROM %d, value:%d\n", src->regs[rs], get_imm_signed(raw_inst), rd, src->mem[addr]);
	src->PC++;
}

void jr	(simulator_context* src, reg_e rd, reg_e rs, reg_e rt, char* imm, int32_t raw_inst) {
	if (!src) {
		return;
	}
	src->PC = src->regs[rd];
	fprintf(stderr, "JR - NEW PC IS %d\n", src->PC);
}

void binary_to_line(int32_t instr,char* buffer) {
	for (int i = 0; i < WORD_HEX_SIZE; i++) {
		int curr_quad = (instr >> (28 - 4 * i))&MASK_4_BIT;
		//itoa(curr_quad, buffer +i, 16);
		sprintf(buffer, "%08X", instr);
	}
}

//TODO: ADD TRUNCATION TO LAST "\n" char.
SIMULATOR_STATUS write_regout(simulator_context* src) {
	if (!src) { return SIM_INVALID_ARGUMENT; }
	char buff[WORD_HEX_SIZE + 1];
	buff[WORD_HEX_SIZE] = '\0';
	for (int i = 0; i < REG_NUM; i++) {
		binary_to_line(src->regs[i], buff);
		if (fprintf(src->regout, "%s\n", buff ) < 0) {
			return SIM_IO_ERROR;
		}
		fflush(src->regout);
	}
	return SIM_SUCCESS;
}

SIMULATOR_STATUS write_memout(simulator_context* src) {
	if (!src) { return SIM_INVALID_ARGUMENT; }
	char buff[WORD_HEX_SIZE + 1];
	buff[WORD_HEX_SIZE] = '\0';
	for (int i = 0; i < MAX_MEM_ADDRESS; i++) {
		binary_to_line(src->mem[i], buff);
		if (fprintf(src->memout, "%s\n", buff) < 0) {
			return SIM_IO_ERROR;
		}
		fflush(src->memout);
	}
	return SIM_SUCCESS;
}

SIMULATOR_STATUS update_trace(simulator_context* src) {
	if (!src) { return SIM_INVALID_ARGUMENT; }
	char buff[WORD_HEX_SIZE + 1];
	buff[WORD_HEX_SIZE] = '\0';
	//write PC
	binary_to_line(src->PC, buff);
	if (fprintf(src->trace, "%s ", buff) < 0) {
		return SIM_IO_ERROR;
	}
	//write INST:
	binary_to_line(src->mem[src->PC], buff);
	if (fprintf(src->trace, "%s", buff) < 0) {
		return SIM_IO_ERROR;
	}
	//write registers
	for (int i = 0; i < REG_NUM; i++) {
		binary_to_line(src->regs[i], buff);
		if (fprintf(src->trace, " %s", buff) < 0) {
			return SIM_IO_ERROR;
		}
		fflush(src->memout);
	}
	if (fprintf(src->trace, "\n") < 0) {
		return SIM_IO_ERROR;
	}
	return SIM_SUCCESS;
}

SIMULATOR_STATUS start_simulator(simulator_context* src) {
	if (!src) return SIM_INVALID_ARGUMENT;
	int instruction_cout = 0;
	char imm[2];
	inst_impl impl;
	while (1) {
		instruction_cout++;
		int32_t inst = src->mem[src->PC];
		fprintf(stderr, "INST: %d\n", inst);
		//immediate
		imm[1] = inst & MASK_8_BIT;
		imm[0] = (inst >> 8) & MASK_8_BIT;
		//RT
		reg_e rt = (inst >> 16) & MASK_4_BIT;
		//RS
		reg_e rs = (inst >> 20) & MASK_4_BIT;
		//RD
		reg_e rd = (inst >> 24) & MASK_4_BIT;
		//OPCODE
		op_code_e opcode = (inst >> 28) & MASK_4_BIT;
		//update the trace file.
		if (update_trace(src) != SIM_SUCCESS) return SIM_IO_ERROR;
		//get the function that implements this opcode.
		impl = get_instruction_impl(opcode);
		if (opcode == HALT) {
			break;
		}
		if (impl == NULL) break; // this was a HALT instruction.
		//perform the instruction.
		impl(src, rd, rs, rt, imm,inst);
	}
	if (write_regout(src) != SIM_SUCCESS) return SIM_IO_ERROR;
	if (write_memout(src) != SIM_SUCCESS) return SIM_IO_ERROR;
	if (fprintf(src->count, "%d", instruction_cout) < 0) return SIM_IO_ERROR;
	return SIM_SUCCESS;
}