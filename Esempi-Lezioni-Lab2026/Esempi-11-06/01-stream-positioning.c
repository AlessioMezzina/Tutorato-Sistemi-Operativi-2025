/**
 * mostra come controllare il file offset associato ad uno stream usando
 * `fseeko`, `ftello` e `rewind`
 *
 * > gcc -std=c99 Esempi-11-06/01-stream-positioning.c examples/lib-misc.c -o Esempi-11-06/01-stream-positioning
 * > ./Esempi-11-06/01-stream-positioning Esempi-11-06/file.txt
 */

#include "../examples/lib-misc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

void print_char(int c) {
    if (c == EOF)
        printf("EOF");
    else if (c == '\n')
        printf("'\\n'");
    else if (c == '\t')
        printf("'\\t'");
    else if (isprint(c))
        printf("'%c'", c);
    else
        printf("codice %d", c);
}

void read_and_print_char(FILE *in, const char *label) {
    off_t before, after;
    int c;

    if ((before = ftello(in)) == -1)
        exit_with_sys_err("ftello");

    c = fgetc(in);

    if ((after = ftello(in)) == -1)
        exit_with_sys_err("ftello");

    printf("[main] %s: offset prima=%lld, carattere=", label,
           (long long)before);
    print_char(c);
    printf(", offset dopo=%lld\n", (long long)after);
}

int main(int argc, char *argv[]) {
    FILE *in;
    off_t size;

    if (argc != 2)
        exit_with_err_msg("utilizzo: %s <file>\n", argv[0]);

    if ((in = fopen(argv[1], "rb")) == NULL)
        exit_with_sys_err(argv[1]);

    // calcola la dimensione spostandosi temporaneamente alla fine del file
    if (fseeko(in, 0, SEEK_END) == -1)
        exit_with_sys_err("fseeko");
    if ((size = ftello(in)) == -1)
        exit_with_sys_err("ftello");
    printf("[main] dimensione stimata del file: %lld byte\n",
           (long long)size);

    if (size == 0)
        exit_with_err_msg("file vuoto: nessun carattere da mostrare\n");

    // lettura dal primo byte
    rewind(in);
    read_and_print_char(in, "lettura dall'inizio");

    // lettura da circa metà file
    if (fseeko(in, size / 2, SEEK_SET) == -1)
        exit_with_sys_err("fseeko");
    read_and_print_char(in, "lettura da meta' file");

    // lettura dell'ultimo byte
    if (fseeko(in, -1, SEEK_END) == -1)
        exit_with_sys_err("fseeko");
    read_and_print_char(in, "lettura dell'ultimo byte");

    // ritorno all'inizio tramite rewind
    rewind(in);
    read_and_print_char(in, "lettura dopo rewind");

    fclose(in);

    exit(EXIT_SUCCESS);
}
