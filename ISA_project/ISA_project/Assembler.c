
//TODO: CHECK HOW MANY LINES ARE WRITTEN TO MEMIN.TXT (I HAD A FILE WITH 65537|)
#define MAX_STRING 512 //MAX LENGTH 
#define MAX_LABEL 60
#define SIZE_OF_MEMORY_IN_DWORD 65536
#define _CRT_SECURE_NO_WARNINGS 

#include "Assembler.h"
#include <stdbool.h>

bool debug_mode = false;

void debug(const char* command, const char* message, const char* additional_info) {
	if (debug_mode)
		printf("\ncommand is %s  and it was parsed as %s  and additional info is %s\n",command,message, additional_info);
}

labels_s* assembly_parse_file_labels(FILE* asm_in) {
	int num_labels = 0, array_size = 512, file_line_number = 0;
	labels_s* labels = (labels_s*)malloc(sizeof(labels_s));
	if (labels == NULL) return NULL;
	label_s** labels_raw = (label_s**)malloc(sizeof(label_s*) * array_size);
	if (labels_raw == NULL) return NULL;
	char BUFFER[MAX_STRING];

	while (fgets(BUFFER, MAX_STRING, asm_in)) {
		char label_line[MAX_STRING]; char op_code[MAX_STRING];
		memset(label_line, '\0', MAX_STRING * sizeof(char));
		memset(op_code, '\0', MAX_STRING * sizeof(char));
		int is_empty_line = sscanf(BUFFER, "%s", label_line);
		if (is_empty_line == EOF || label_line[0] == '#' || !strcmp(label_line,".word")) {
			file_line_number--;
			debug(label_line, "empty line","");
		}

		else if (strlen(label_line) > 0 && label_line[strlen(label_line) - 1] == ':' 
			&& label_line[0] != '#') {
			label_s* label = (label_s*)malloc(sizeof(label_s));
			if (label == NULL) return NULL;
			label->label_str = (char*)malloc(strlen(label_line) * sizeof(char));
			strncpy(label->label_str, label_line, strlen(label_line) - 1);
			label->label_str[strlen(label_line) - 1] = '\0';
			label->line_number = file_line_number;
			labels_raw[num_labels] = label;
			num_labels++;

			memset(label_line, '\0', MAX_STRING * sizeof(char));
			int is_label_and_op = sscanf(BUFFER, "%s %s", label_line, op_code);
			if(is_label_and_op == 1 || 
				(is_label_and_op == 2 && strlen(op_code) > 0 && op_code[0] == '#'))  //only label
				file_line_number--; /*the line number should not be increased for a label line*/
			
			if (num_labels == array_size) {
				array_size *= 2;
				labels_raw = realloc(labels_raw, sizeof(label_s*) * array_size);
				if (labels_raw == NULL) return NULL;
			}
			debug(label_line, "a label line","");
		}
		else {
			debug(label_line, "not a label line", "");
		}
		file_line_number++;
	}
	labels_raw = realloc(labels_raw, sizeof(label_s*) * num_labels);
	labels->labels = labels_raw;
	labels->num_of_labels = num_labels;
	return labels;	
}

STATUS_E parse_lines(FILE* asm_in, FILE* mem_out, labels_s* labels) {
	int file_line_number = 0, cur_max_line = 0;
	rewind(asm_in);
	char** mem_shadow = (char**)malloc(sizeof(char*)*SIZE_OF_MEMORY_IN_DWORD);
	memset(mem_shadow, 0, sizeof(char*)*SIZE_OF_MEMORY_IN_DWORD);

	char op_str[MAX_LABEL], rd_str[MAX_LABEL], rs_str[MAX_LABEL], rt_str[MAX_LABEL], imm_str[MAX_LABEL];
	char BUFFER[MAX_STRING];
	while (fgets(BUFFER, MAX_STRING, asm_in)) {
		memset(op_str, 0, sizeof(char)*MAX_LABEL);
		memset(rd_str, 0, sizeof(char)*MAX_LABEL);
		memset(rs_str, 0, sizeof(char)*MAX_LABEL);
		memset(rt_str, 0, sizeof(char)*MAX_LABEL);
		memset(imm_str, 0, sizeof(char)*MAX_LABEL);
		int is_empty_line = sscanf(BUFFER, "%s", op_str);
		if (is_empty_line == EOF || op_str[0] == '#') {
			file_line_number--;
			debug(BUFFER, "empty line or comment line", op_str);
		}
		
		//for label line delete the label, and then parse
		else if (strlen(op_str) > 0 && op_str[strlen(op_str) - 1] == ':'
			&& op_str[0] != '#') {

			memset(op_str, '\0', MAX_LABEL * sizeof(char));
			int is_label_and_op = sscanf(BUFFER, "%s %s", op_str, rd_str);
			if (is_label_and_op == 1 ||
				(is_label_and_op == 2 && strlen(rd_str) > 0 && rd_str[0] == '#')) {  //only label
				continue;
			}
			else { // it's a label and an operation
				strcpy(BUFFER, BUFFER + strlen(op_str));
				memset(op_str, '\0', MAX_LABEL * sizeof(char));
				memset(rd_str, '\0', MAX_LABEL * sizeof(char));
				sscanf(BUFFER, "%s", op_str);
			}
			debug(BUFFER, "label inside parse lines", op_str);
		}


		if (!strcmp(op_str, ".word")) {
			int16_t line = 0, value_int = 0;
			sscanf(BUFFER, "%s %s %s", op_str, rd_str, rs_str);
			if (strstr(rd_str, "0x") || strstr(rd_str, "0X")) { line = (int)strtol(rd_str, NULL, 16); }
			else { line = (int16_t)strtol(rd_str, NULL, 10); }
			if (strstr(rs_str, "0x") || strstr(rs_str, "0X")) { value_int = (int)strtol(rs_str, NULL, 16); }
			else { value_int = (int16_t)strtol(rs_str, NULL, 10); }
			STATUS_E status = write_to_tf(line, 
				int_to_hex_string((unsigned)value_int),mem_shadow);
			if (status != SUCCESS) return status;
			file_line_number--; /*the line will be increased in the loop exit*/
			cur_max_line = (cur_max_line < line) ? line : cur_max_line;
			debug(BUFFER, "word command", "");
		}

		else if (!strcmp(op_str, "add") || !strcmp(op_str, "sub") || !strcmp(op_str, "and") ||
			!strcmp(op_str, "or") || !strcmp(op_str, "sll") || !strcmp(op_str, "sra")) {
			char* value;
			sscanf(BUFFER, "%s %s %s %s %s", op_str, rd_str, rs_str, rt_str, imm_str);
			int rd_idx = get_reg_index(rd_str);
			int rs_idx = get_reg_index(rs_str);
			int rt_idx = get_reg_index(rt_str);
			int op_idx = get_op_index(op_str);
			int16_t imm_val;
			if (strlen(imm_str)>0 && isalpha(imm_str[0]) &&
				(imm_val = imm_is_label(imm_str, labels)) != NO_LABEL_FOUND) {
				imm_val = imm_val;
			}
			else if (strstr(imm_str, "0x") || strstr(imm_str, "0X")) { imm_val = (int16_t)strtol(imm_str, NULL, 16); }
			else { imm_val = (int16_t)strtol(imm_str, NULL, 10); }
			value = get_value_from_op_strs(op_idx, rd_idx, rs_idx, rt_idx, imm_val /*no immediate*/);
			STATUS_E status = write_to_tf(file_line_number, value,mem_shadow); if (status != SUCCESS) return status;
			debug(BUFFER, "add sub and or sll sra command", value);
		}

		
		else if (!strcmp(op_str, "limm") || !strcmp(op_str, "beq") || !strcmp(op_str, "bgt") || !strcmp(op_str, "ble")
			|| !strcmp(op_str, "bne") || !strcmp(op_str, "lw") || !strcmp(op_str, "sw") || !strcmp(op_str, "jal")) {
			int line = 0;
			char* value;
			sscanf(BUFFER, "%s %s %s %s %s", op_str, rd_str, rs_str, rt_str, imm_str);
			int rd_idx = get_reg_index(rd_str);
			int rs_idx = get_reg_index(rs_str);
			int rt_idx = get_reg_index(rt_str);
			int op_idx = get_op_index(op_str);
			int16_t imm_val;
			if (strlen(imm_str)>0 && isalpha(imm_str[0]) &&
				(imm_val = imm_is_label(imm_str,labels)) != NO_LABEL_FOUND) { imm_val = imm_val; }
			else if (strstr(imm_str, "0x") || strstr(imm_str, "0X")) { imm_val = (int16_t)strtol(imm_str, NULL, 16); }
			else { imm_val = (int16_t)strtol(imm_str, NULL, 10); }
			value = get_value_from_op_strs(op_idx, rd_idx, rs_idx, rt_idx, imm_val);
			STATUS_E status = write_to_tf(file_line_number, value, mem_shadow); if (status != SUCCESS) return status;
			debug(BUFFER, "limm beq bgt ble bne lw sw jal command", value);
		}

		else if (!strcmp(op_str, "jr") || !strcmp(op_str, "halt")) {
			int line = 0;
			char* value;
			sscanf(BUFFER, "%s %s %s %s %s", op_str, rd_str, rs_str, rt_str, imm_str);
			int rd_idx = get_reg_index(rd_str);
			int rs_idx = get_reg_index(rs_str);
			int rt_idx = get_reg_index(rt_str);
			int op_idx = get_op_index(op_str);
			int16_t imm_val = 0;
			if (strlen(imm_str)>0 && isalpha(imm_str[0]) &&
				(imm_val = imm_is_label(imm_str, labels)) != NO_LABEL_FOUND) {
				imm_val = imm_val;
			}
			value = get_value_from_op_strs(op_idx, rd_idx, rs_idx, rt_idx, imm_val);
			STATUS_E status = write_to_tf(file_line_number, value, mem_shadow); if (status != SUCCESS) return status;
			debug(BUFFER, "jr command", value);
		}
		else {
			debug(BUFFER, "could not find any match - ---BAD---", "");
		}
		file_line_number++;
	}
	file_line_number--;
	cur_max_line = (cur_max_line < file_line_number) ? file_line_number : cur_max_line;

	for (int i = 0; i <= cur_max_line; ++i) {
		if (mem_shadow[i] == NULL) {
			char* zero_memory = int_to_hex_string(0);
			fputs(zero_memory, mem_out);
			fputc('\n', mem_out);
			free(zero_memory);
		}
		else {
			fputs(mem_shadow[i], mem_out);
			fputc('\n', mem_out);
			free(mem_shadow[i]);
		}
	}
	free(mem_shadow);

	return SUCCESS;
}

STATUS_E release_labels(labels_s* labels) {
	for (int i = 0; i < labels->num_of_labels; ++i) {
		free(labels->labels[i]->label_str);
		free(labels->labels[i]);
	}
	free(labels->labels);
	free(labels);
	return SUCCESS;
}

STATUS_E write_to_tf(int line_num, char* value, char** mem_shadow)
{
	mem_shadow[line_num] = value;
	return SUCCESS;
}

int get_reg_index(char * reg_str)
{
	if (reg_str != NULL && strlen(reg_str) > 0 && reg_str[strlen(reg_str) - 1] == ',')
		reg_str[strlen(reg_str) - 1] = '\0';
	if (!strcmp(reg_str, "$zero")) return 0;
	if (!strcmp(reg_str, "$at")) return 1;
	if (!strcmp(reg_str, "$v0")) return 2;
	if (!strcmp(reg_str, "$a0")) return 3;
	if (!strcmp(reg_str, "$a1")) return 4;
	if (!strcmp(reg_str, "$t0")) return 5;
	if (!strcmp(reg_str, "$t1")) return 6;
	if (!strcmp(reg_str, "$t2")) return 7;
	if (!strcmp(reg_str, "$t3")) return 8;
	if (!strcmp(reg_str, "$s0")) return 9;
	if (!strcmp(reg_str, "$s1")) return 10;
	if (!strcmp(reg_str, "$s2")) return 11;
	if (!strcmp(reg_str, "$gp")) return 12;
	if (!strcmp(reg_str, "$sp")) return 13;
	if (!strcmp(reg_str, "$fp")) return 14;
	if (!strcmp(reg_str, "$ra")) return 15;

	debug(reg_str, "inside get_reg_index and no good was found ---BAD---", "");
	return INVALID_VALUE;
}

int get_op_index(char * op_str)
{
	if (!strcmp(op_str, "add")) return 0;
	if (!strcmp(op_str, "sub")) return 1;
	if (!strcmp(op_str, "and")) return 2;
	if (!strcmp(op_str, "or")) return 3;
	if (!strcmp(op_str, "sll")) return 4;
	if (!strcmp(op_str, "sra")) return 5;
	if (!strcmp(op_str, "limm")) return 6;
	if (!strcmp(op_str, "beq")) return 7;
	if (!strcmp(op_str, "bgt")) return 8;
	if (!strcmp(op_str, "ble")) return 9;
	if (!strcmp(op_str, "bne")) return 10;
	if (!strcmp(op_str, "jal")) return 11;
	if (!strcmp(op_str, "lw")) return 12;
	if (!strcmp(op_str, "sw")) return 13;
	if (!strcmp(op_str, "jr")) return 14;
	if (!strcmp(op_str, "halt")) return 15;

	debug(op_str, "inside get_op_str and no good was found ---BAD---", "");
	return INVALID_VALUE;
}

int imm_is_label(char * imm_str, labels_s * labels)
{
	for (int i = 0; i < labels->num_of_labels; ++i) {
		if (!strcmp(imm_str, labels->labels[i]->label_str)) {
			debug(imm_str, "in imm_is_label and found label line", "");
			return labels->labels[i]->line_number;
		}
			
	}
	
	return NO_LABEL_FOUND;
}

char* int_to_hex_string(unsigned n)
{
	char* val = malloc(sizeof(char) * 9);
	memset(val, '\0', 9);
	sprintf(val, "%08X", n);
	return val;
}

char* get_value_from_op_strs(int op_idx, int rd_idx, int rs_idx, int rt_idx, int16_t immediate)
{
	int value =  0 | (op_idx << 28) | (rd_idx << 24) | (rs_idx << 20) | (rt_idx << 16) | (0xffff & immediate);
	return int_to_hex_string((unsigned int)(value));
}

STATUS_E assembly_to_binary(char *asm_in_str, char *mem_out_str)
{
	FILE *asm_in, *mem_out;
	asm_in = fopen(asm_in_str, "r");
	mem_out = fopen(mem_out_str, "w");

	labels_s* labels = assembly_parse_file_labels(asm_in);
	STATUS_E status = parse_lines(asm_in, mem_out,labels);
	if (status != SUCCESS) return status;

	release_labels(labels);
	fclose(asm_in);
	fclose(mem_out);
	return SUCCESS;

}
