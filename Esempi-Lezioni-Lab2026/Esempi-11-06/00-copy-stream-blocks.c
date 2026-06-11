/**
 * copia un file sorgente su uno destinazione usando gli stream e trasferendo i
 * dati per blocchi con `fread` e `fwrite`
 *
 * > gcc -std=c99 Esempi-11-06/00-copy-stream-blocks.c examples/lib-misc.c -o Esempi-11-06/00-copy-stream-blocks
 * > ./Esempi-11-06/00-copy-stream-blocks Esempi-11-06/file.txt Esempi-11-06/file.copia.txt
 */

#include "../examples/lib-misc.h"
#include <stdio.h>
#include <stdlib.h>

#define RECORD_SIZE 1
#define NUM_RECORDS BUFSIZ

int main(int argc, char *argv[]) {
    FILE *in, *out;
    char buffer[RECORD_SIZE * NUM_RECORDS];
    size_t records_read, records_written;
    unsigned long blocks = 0, total_bytes = 0;

    if (argc != 3)
        exit_with_err_msg("utilizzo: %s <sorgente> <destinazione>\n", argv[0]);

    printf("creo una copia di '%s' su '%s'\n", argv[1], argv[2]);

    // apre lo stream sorgente in lettura
    if ((in = fopen(argv[1], "rb")) == NULL)
        exit_with_sys_err(argv[1]);

    // apre/crea lo stream destinazione in scrittura (con troncamento)
    if ((out = fopen(argv[2], "wb")) == NULL)
        exit_with_sys_err(argv[2]);

    // copia i dati dalla sorgente alla destinazione a blocchi
    while ((records_read = fread(buffer, RECORD_SIZE, NUM_RECORDS, in)) > 0) {
        blocks++;
        total_bytes += records_read * RECORD_SIZE;

        printf("[main] blocco %lu: letti %zu record da %d byte\n", blocks, // %lu per unsigned long, %zu per size_t
               records_read, RECORD_SIZE);

        records_written = fwrite(buffer, RECORD_SIZE, records_read, out);
        if (records_written != records_read) {
            if (ferror(out))
                exit_with_sys_err(argv[2]);
            exit_with_err_msg("scrittura incompleta su '%s'\n", argv[2]);
        }
    }

    // fread ritorna 0 sia a fine file sia in caso di errore: bisogna controllare
    if (ferror(in)) //ferror controlla se c'è stato un errore di lettura
        exit_with_sys_err(argv[1]);

    printf("[main] copia completata: %lu blocchi, %lu byte trasferiti\n",
           blocks, total_bytes);

    fclose(in);
    fclose(out);

    exit(EXIT_SUCCESS);
}
