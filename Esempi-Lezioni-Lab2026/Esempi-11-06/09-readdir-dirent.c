/**
 * legge tutte le voci di una directory con `readdir` e mostra i campi
 * principali della struttura `dirent`: `d_ino` e `d_name`
 *
 * > gcc -std=c99 Esempi-11-06/09-readdir-dirent.c examples/lib-misc.c -o Esempi-11-06/09-readdir-dirent
 * > ./Esempi-11-06/09-readdir-dirent Esempi-11-06
 */

#include "../examples/lib-misc.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    DIR *dir;
    struct dirent *entry;
    const char *dirname;
    unsigned long count = 0;

    if (argc > 2)
        exit_with_err_msg("utilizzo: %s [directory]\n", argv[0]);

    dirname = (argc == 2 ? argv[1] : ".");

    if ((dir = opendir(dirname)) == NULL)
        exit_with_sys_err(dirname);

    printf("[main] lettura delle voci di '%s'\n", dirname);
    while (1) {
        errno = 0;
        entry = readdir(dir);

        if (entry == NULL) {
            if (errno != 0)
                exit_with_sys_err("readdir");
            break; // fine directory
        }

        printf("  voce %lu: d_ino=%lu d_name='%s'\n", ++count,
               (unsigned long)entry->d_ino, entry->d_name);
    }

    printf("[main] fine directory: %lu voci lette\n", count);

    if (closedir(dir) == -1)
        exit_with_sys_err("closedir");

    exit(EXIT_SUCCESS);
}
