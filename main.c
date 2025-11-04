#include "urm.h"
#include <stdio.h>

int main(void) {
    const char *prg_file = "program.urm";

    FILE *fp = fopen(prg_file, "r");
    fseek(fp, 0L, SEEK_END);
    long prg_file_len = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char prg_text[prg_file_len + 1];
    fread(prg_text, prg_file_len, 1, fp);
    prg_text[prg_file_len] = '\0';
    fclose(fp);

    URM *urm = urm_init(prg_text);

    unsigned int input[] = {1, 10};
    printf("RES = %u\n", urm_exec(urm, input, sizeof(input)/sizeof(input[0])));

    urm_free(urm);
}
