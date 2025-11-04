#ifndef URM_INTERPRETER
#define URM_INTERPRETER

#include <stddef.h>

typedef struct URM URM;

URM *urm_init(const char *prg_text);

void urm_free(URM *urm);

unsigned int urm_exec(URM *urm, unsigned int *input, size_t input_len);

#endif /* URM_INTERPRETER */
