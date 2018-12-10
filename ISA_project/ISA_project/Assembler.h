#pragma once

//specify the maximum line length
#define MAX_LINE_LENGTH 1024
#define NO_LABEL_FOUND -1
#define INVALID_VALUE -1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "reverse_assembler.h"
#include <stdint.h>

typedef enum {
	INVALID_ARGS,
	MEMORY_FAILIURE,
	SUCCESS,
}STATUS_E;

typedef struct {
	char* label_str;
	int line_number;
}label_s;

typedef struct {
	label_s** labels;
	int num_of_labels;
}labels_s;


char* int_to_hex_string(unsigned n);

labels_s* assembly_parse_file_labels(FILE* asm_in);

STATUS_E assembly_to_binary(char *asm_in_str, char *mem_out_str);

STATUS_E release_labels(labels_s* labels);


STATUS_E write_to_tf(int line_num, char* value, char** mem_shadow);

STATUS_E parse_lines(FILE* asm_in, FILE* mem_out,labels_s* labels);

int get_reg_index(char* reg_str);

int get_op_index(char* op_str);

char* get_value_from_op_strs(int op_idx, int  rd_idx, int  rs_idx, int  rt_idx, int16_t immediate);

int imm_is_label(char* imm_str, labels_s* labels);

