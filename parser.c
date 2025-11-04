#include "parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

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
void urm_parse_prg(URM_instr *instructions, const char *prg_text) {
    size_t line = 0;

    const char *p = prg_text;
    while (*p != '\0') {
        skip_spaces(&p);

        if (*p == '\n') {
            p++;
            continue;
        }

        /* Get instr type */
        URM_instr *i = instructions + line;
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

size_t urm_get_program_instr_number(const char *prg_text) {
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

    return prg_lines;
}
