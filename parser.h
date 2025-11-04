#ifndef URM_PARSER
#define URM_PARSER

#include <stddef.h>

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

/*
Parses the program and put the instruction into the 'instructions' array.
*/
void urm_parse_prg(URM_instr *instructions, const char *prg_text);

/*
Returns the number of instruction of program 'prg_text'.
*/
size_t urm_get_program_instr_number(const char *prg_text);

#endif /* URM_PARSER */
