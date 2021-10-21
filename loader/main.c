#include <stdio.h>

#include "loader.h"

int main(void) {
	Program* program = program_load("target/target.elf");
	run(program);
	program_free(program);
	return 0;
}