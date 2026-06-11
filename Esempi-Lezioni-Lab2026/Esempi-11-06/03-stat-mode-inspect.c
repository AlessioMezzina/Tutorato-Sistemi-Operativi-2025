/**
 * ispeziona il campo `st_mode` di un oggetto del file-system, separando la
 * maschera dei permessi dal tipo di oggetto tramite le macro `S_IS...`
 *
 * > gcc -std=c99 Esempi-11-06/03-stat-mode-inspect.c examples/lib-misc.c -o Esempi-11-06/03-stat-mode-inspect
 * > ./Esempi-11-06/03-stat-mode-inspect Esempi-11-06/file.txt
 * > ./Esempi-11-06/03-stat-mode-inspect Esempi-11-06
 * > ln -sf file.txt Esempi-11-06/link-file.txt
 * > ./Esempi-11-06/03-stat-mode-inspect Esempi-11-06/link-file.txt
 */

#include "../examples/lib-misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#define PERMISSION_MASK 0777

void print_predicate(const char *name, int value) {
    printf("  %-10s : %s\n", name, value ? "vero" : "falso");
}

int main(int argc, char *argv[]) {
    struct stat statbuf;
    mode_t modes;

    if (argc != 2)
        exit_with_err_msg("utilizzo: %s <pathname>\n", argv[0]);

    // usa `lstat` per osservare un eventuale link simbolico senza attraversarlo
    if (lstat(argv[1], &statbuf) == -1)
        exit_with_sys_err(argv[1]);

    modes = statbuf.st_mode;


    printf("pathname: %s\n", argv[1]);
    printf("st_mode completo : %o\n", modes);
    printf("permessi         : %o\n", modes & PERMISSION_MASK);

    printf("\ntipo di oggetto:\n"); 
    print_predicate("S_ISREG", S_ISREG(modes));
    print_predicate("S_ISDIR", S_ISDIR(modes));
    print_predicate("S_ISBLK", S_ISBLK(modes));
    print_predicate("S_ISCHR", S_ISCHR(modes));
    print_predicate("S_ISLNK", S_ISLNK(modes));

    exit(EXIT_SUCCESS);
}
