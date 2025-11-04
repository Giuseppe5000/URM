#include "urm.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*==================================== Data structures ====================================*/

#define URM_INIT_MEMORY_CAP 256 /* Initial allocated memory for the machine */

struct URM {
    size_t pc; /* Program counter */

    unsigned int *memory;
    size_t memory_capacity;

    URM_instr *instructions;
    size_t instructions_len;
};

/*=========================================================================================*/


/*========================================= Utils =========================================*/

/*
Wrapping malloc with this helper function,
handling OOM with exit and logging the error.
*/
static void *urm_malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "[ERROR]: Out of memory.");
        exit(1);
    }
    return ptr;

}

/*
Same as urm_malloc.
*/
static void *urm_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if (ptr == NULL) {
        fprintf(stderr, "[ERROR]: Out of memory.");
        exit(1);
    }
    return ptr;
}

/*
Same as urm_malloc.
*/
void *urm_realloc(void *ptr, size_t size) {
    void *p = realloc(ptr, size);
    if (p == NULL) {
        fprintf(stderr, "[ERROR]: Out of memory.");
        exit(1);
    }
    return p;
}

static unsigned int max(unsigned int a, unsigned int b) {
    return a > b ? a : b;
}

/*=========================================================================================*/


URM *urm_init(const char *prg_text) {
    URM *urm = urm_malloc(sizeof(URM));
    urm->pc = 1; /* First instruction is always 1 */
    urm->memory = urm_calloc(URM_INIT_MEMORY_CAP, sizeof(unsigned int));
    urm->memory_capacity = URM_INIT_MEMORY_CAP;

    /* Getting program instructions num */
    size_t prg_lines = urm_get_program_instr_number(prg_text);

    urm->instructions_len = prg_lines;
    urm->instructions = urm_malloc(urm->instructions_len * sizeof(URM_instr));

    urm_parse_prg(urm->instructions, prg_text);
    return urm;
}

void urm_free(URM *urm) {
    free(urm->instructions);
    free(urm->memory);
    free(urm);
}

unsigned int urm_exec(URM *urm, unsigned int *input, size_t input_len) {
    /* Loads the input into the machine memory */
    memcpy(urm->memory, input, input_len*sizeof(unsigned int));

    /* Start execution */
    while (urm->pc - 1 < urm->instructions_len) {
        const URM_instr *current_instr = urm->instructions + urm->pc - 1;
        unsigned int arg1 = current_instr->args[0];
        unsigned int arg2 = current_instr->args[1];
        unsigned int arg3 = current_instr->args[2];

        /* Enlarge memory if needed */
        unsigned int max_arg = max(arg1, arg2);
        while (max_arg - 1 >= urm->memory_capacity) {
            size_t old_capacity = urm->memory_capacity;
            urm->memory_capacity *= 2;
            urm->memory = urm_realloc(urm->memory, urm->memory_capacity*sizeof(unsigned int));

            /* Setting new memory to 0, because realloc doesn't do that */
            memset(urm->memory + old_capacity, 0, (urm->memory_capacity - old_capacity)*sizeof(unsigned int));
        }

        switch (current_instr->type) {
            case ZERO:
                urm->memory[arg1 - 1] = 0;
                urm->pc++;
                break;
            case SUCC:
                urm->memory[arg1 - 1] += 1;
                urm->pc++;
                break;
            case TRANSFER:
                urm->memory[arg2 - 1] = urm->memory[arg1 - 1];
                urm->pc++;
                break;
            case JMP:
                if (urm->memory[arg1 - 1] == urm->memory[arg2 - 1]) {
                    urm->pc = arg3;
                } else {
                    urm->pc++;
                }
                break;
            default:
                fprintf(stderr, "[ERROR]: Invalid instruction\n");
                exit(1);
        }
    }

    /* Execution terminate, returning the result (R1) */
    return urm->memory[0];
}
