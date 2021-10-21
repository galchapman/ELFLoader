#include "loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <elf.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

struct _program {
	Elf64_Ehdr header;
	Elf64_Phdr* pheaders;
	void** pheaders_values;
	Elf64_Shdr* sections;
};

static void* LoadProgramHeader(int fd, const Elf64_Phdr* header) {
	off_t offset, pa_offset;
	size_t length;

	offset = header->p_offset;
	length = header->p_memsz;

	pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
	off_t inpage_offset = offset - pa_offset;

	return mmap((void*)header->p_paddr, length + inpage_offset, PROT_READ | PROT_EXEC, MAP_PRIVATE, fd, pa_offset) + inpage_offset;
}

static void UnloadProgramHeader(void * addr, const Elf64_Phdr* header) {
	off_t offset, pa_offset;
	size_t length;

	offset = header->p_offset;
	length = header->p_memsz;

	pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
	off_t inpage_offset = offset - pa_offset;
	munmap(addr - inpage_offset, length + inpage_offset);
}

Program* program_load(const char* filename) {
	Program* program = (Program*)malloc(sizeof(Program));
	int fd = open(filename, O_RDONLY);
	FILE* file = fdopen(fd, "rb");
	if (!file)
		goto error;
	// Read Elf header
	fread(&program->header, sizeof(Elf64_Ehdr), 1, file);
	// Read Elf program headers
	fseek(file, program->header.e_phoff, SEEK_SET);
	program->pheaders = (Elf64_Phdr*)malloc(sizeof(Elf64_Phdr) * program->header.e_phnum);
	fread(program->pheaders, sizeof(Elf64_Phdr), program->header.e_phnum, file);
	// maps program headers sections
	program->pheaders_values = (void**)malloc(sizeof(void*) * program->header.e_phnum);
	for (int i = 0; i < program->header.e_phnum; i++) {
		program->pheaders_values[i] = LoadProgramHeader(fd, program->pheaders+i);
	}

	// Read Elf sections
	fseek(file, program->header.e_shoff, SEEK_SET);
	program->sections = (Elf64_Shdr*)malloc(sizeof(Elf64_Shdr) * program->header.e_shnum);
	fread(program->sections, sizeof(Elf64_Shdr), program->header.e_shnum, file);

	fclose(file);
	return program;

error:
	free(program);
	return NULL;
}

void program_free(Program* program) {
	free(program->pheaders);
	for (int i = 0; i < program->header.e_phnum; i++) {
		UnloadProgramHeader(program->pheaders_values[i], program->pheaders+i);
	}
	free(program->sections);
	free(program->pheaders_values);
	free(program);
}

void run(Program* program) {
	void (*func)(void) = (void*)program->header.e_entry;
	func();
}