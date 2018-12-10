#include "reverse_assembler.h"

command_s parse_instruction(int32_t instr) {
	command_s res;
	//IMM:
	int32_t imm32 = instr & IMM_MASK;
	res.imm[1] = imm32 & ((1 << 8) -1);
	res.imm[2] = (imm32 >> 8) & ((1 << 8) -1);
	//opcode: get 4 left most bits.
	res.op_code = (instr>>28) & MASK_4_BIT;
	//RD: get 4 bits from 24 to 27 (incl.)
	res.rd = (instr>>24) & MASK_4_BIT;
	//RS: get 4 bits from 20 to 26 (incl.)
	res.rs = (instr >> 20 )& MASK_4_BIT;
	//RT: get 4 bits from 16 to 19 (incl.)
	res.rt = (instr >> 16)  & MASK_4_BIT;
	return res;
}