#ifndef URM_INTERPRETER
#define URM_INTERPRETER

#include <stddef.h>

typedef struct URM URM; /* Opaque type */

/*
Parses the URM program inside 'prg_text' and
alloc all the necessary memory for the machine for execute.
*/
URM *urm_init(const char *prg_text);

/*
Freeing allocated memory.

[NOTE]: Using 'urm' after calling this function, obviously, causes undefined behaviour.
*/
void urm_free(URM *urm);

/*
Excecute (intepreting) the instructions starting with memory initialised as [input[0], .., input[input_len - 1], 0, 0, 0, ..., 0].
*/
unsigned int urm_exec(URM *urm, unsigned int *input, size_t input_len);

#endif /* URM_INTERPRETER */
