/**
 * lista le voci di una directory usando `opendir`, `readdir` e `closedir`;
 * mostra anche l'uso elementare di `telldir`, `seekdir` e `rewinddir`
 *
 * > gcc -std=c99 Esempi-11-06/08-list-dir-stream.c examples/lib-misc.c -o Esempi-11-06/08-list-dir-stream
 * > ./Esempi-11-06/08-list-dir-stream Esempi-11-06
 */

#include "../examples/lib-misc.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void print_next_entry(DIR *dir, const char *label) {
    struct dirent *entry;

    errno = 0;
    if ((entry = readdir(dir)) == NULL) {
        if (errno != 0)
            exit_with_sys_err("readdir");
        printf("  %s: fine directory\n", label);
        return;
    }

    printf("  %s: cookie-successivo=%ld i-node=%lu nome='%s'\n", label,
           telldir(dir), (unsigned long)entry->d_ino, entry->d_name);
}

int main(int argc, char *argv[]) {
    DIR *dir;
    const char *dirname;

    if (argc > 2)
        exit_with_err_msg("utilizzo: %s [directory]\n", argv[0]);

    dirname = (argc == 2 ? argv[1] : ".");

    printf("[main] apro la directory '%s'\n", dirname);
    if ((dir = opendir(dirname)) == NULL)
        exit_with_sys_err(dirname);

    printf("[main] prime voci lette con readdir:\n");
    print_next_entry(dir, "voce 1");

    long saved_position = telldir(dir);
    printf("[main] salvo con telldir il cookie %ld\n", saved_position);

    print_next_entry(dir, "voce 2");
    print_next_entry(dir, "voce 3");

    printf("\n[main] torno al cookie salvato con seekdir...\n");
    seekdir(dir, saved_position);
    print_next_entry(dir, "voce dopo seekdir");

    printf("\n[main] torno all'inizio con rewinddir...\n");
    rewinddir(dir);

    printf("[main] prime voci dopo rewinddir:\n");
    print_next_entry(dir, "voce 1");
    print_next_entry(dir, "voce 2");

    if (closedir(dir) == -1)
        exit_with_sys_err("closedir");

    exit(EXIT_SUCCESS);
}
