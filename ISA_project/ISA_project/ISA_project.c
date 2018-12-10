#include "Assembler.h"


int main2(int argc, char* argv[])
{
	if (argc != 3) {
		return 1;
	}

	STATUS_E status = assembly_to_binary(argv[1], argv[2]);

	return 0;
}