#include "urm.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*==================================== Data structures ====================================*/

#define URM_INIT_MEMORY_CAP 512 /* Initial allocated memory for the machine */

enum URM_instr_type {
    ZERO,      /* [1 arg] (n) - Puts 0 into the register Rn */
    SUCC,      /* [1 arg] (n) - Increment by 1 the register Rn */
    TRANSFER,  /* [2 args] (m,n) - Transfer content of register Rm into register Rn */
    JMP,       /* [3 args] (m,n,k) - Jump to instruction k if Rm = Rn */
};

typedef struct {
    enum URM_instr_type type;
    unsigned int args[3];    /* Yep, sometimes not all used but idc */
} URM_instr;

struct URM {
    size_t pc; /* Program counter */

    unsigned int *memory;
    size_t memory_capacity;

    URM_instr *instructions;
    size_t instructions_len;
};

/*=========================================================================================*/

/*======================================== Parsing ========================================*/

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

static void *urm_calloc(size_t nmemb, size_t size) {
    void *ptr = calloc(nmemb, size);
    if (ptr == NULL) {
        fprintf(stderr, "[ERROR]: Out of memory.");
        exit(1);
    }
    return ptr;
}

/*=========================================================================================*/

/*======================================== Parsing ========================================*/

/* Skip all type of spaces but \n */
static void skip_spaces(const char **p) {
    while (isspace(**p) && **p != '\n') {
        (*p)++;
    }
}

/* Parsing natural numbers */
static void parse_uint(const char **p, unsigned int n, unsigned int *out) {
    for (size_t i = 0; i < n; ++i) {
        skip_spaces(p);

        if (!isdigit(**p)) {
            fprintf(stderr, "Syntax error: no args\n");
            exit(1);
        }

        const char *num_start = *p;

        /* Skipping the num */
        while(isdigit(**p)) {
            (*p)++;
        }

        out[i] = atoi(num_start);
    }
}

/*
Parse an URM program and puts the instructions (URM_instr) into the 'urm' struct.

The parsing allows spaces everiwhere, the unique constraint is that
on one single line can exists at least one instruction.
*/
static void urm_parse_prg(URM *urm, const char *prg_text) {
    size_t line = 0;

    const char *p = prg_text;
    while (*p != '\0') {
        skip_spaces(&p);

        if (*p == '\n') {
            p++;
            continue;
        }

        /* Get instr type */
        URM_instr *i = urm->instructions + line;
        switch(*p) {
            case 'Z':
                i->type = ZERO;
                p++;
                parse_uint(&p, 1, i->args);
                break;
            case 'S':
                i->type = SUCC;
                p++;
                parse_uint(&p, 1, i->args);
                break;
            case 'T':
                i->type = TRANSFER;
                p++;
                parse_uint(&p, 2, i->args);
                break;
            case 'J':
                i->type = JMP;
                p++;
                parse_uint(&p, 3, i->args);
                break;
            default:
                fprintf(stderr, "Syntax error: getting instr type\n");
                exit(1);
        }

        skip_spaces(&p);
        if (*p == '\0') break;
        if (*p != '\n') {
            fprintf(stderr, "Syntax error: no newline\n");
            exit(1);
        }
        line++;
        p++;
    }
}

/*=========================================================================================*/

URM *urm_init(const char *prg_text) {
    URM *urm = urm_malloc(sizeof(URM));
    urm->pc = 1; /* First instruction is always 1 */
    urm->memory = urm_calloc(URM_INIT_MEMORY_CAP, sizeof(unsigned int));
    urm->memory_capacity = URM_INIT_MEMORY_CAP;

    /* Getting program lines num (excluding empty lines) */
    size_t prg_lines = 0;
    const char *p = prg_text;
    while (*p != '\0') {
        skip_spaces(&p);

        if (*p != '\n') {
            while (*p != '\n' && *p != '\0') {
                p++;
            }
            prg_lines++;
        }

        if (*p != '\0') p++;
    }

    urm->instructions_len = prg_lines;
    urm->instructions = urm_malloc(urm->instructions_len * sizeof(URM_instr));

    urm_parse_prg(urm, prg_text);
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
