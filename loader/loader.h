#ifndef ELF_LOADER_H
#define ELF_LOADER_H

typedef struct _program Program;

#ifdef _cplusplus
extern "C" {
#endif

Program* program_load(const char* file);

void program_free(Program*);

void run(Program*);

#ifdef _cplusplus
}
#endif

#endif //!ELF_LOADER_H