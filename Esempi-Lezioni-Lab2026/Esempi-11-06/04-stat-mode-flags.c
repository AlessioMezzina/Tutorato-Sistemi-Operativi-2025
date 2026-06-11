/**
 * ispeziona `st_mode` usando direttamente le maschere `S_IFMT`, `S_IRWXU`,
 * `S_IRWXG` e `S_IRWXO`
 *
 * > gcc -std=c99 Esempi-11-06/04-stat-mode-flags.c examples/lib-misc.c -o Esempi-11-06/04-stat-mode-flags
 * > ./Esempi-11-06/04-stat-mode-flags Esempi-11-06/file.txt
 * > ./Esempi-11-06/04-stat-mode-flags Esempi-11-06
 * > ln -sf file.txt Esempi-11-06/link-file.txt
 * > ./Esempi-11-06/04-stat-mode-flags Esempi-11-06/link-file.txt
 */

#include "../examples/lib-misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

const char *get_type_name(mode_t modes) {
    switch (modes & S_IFMT) {
    case S_IFBLK:
        return "dispositivo speciale a blocchi";
    case S_IFCHR:
        return "dispositivo speciale a caratteri";
    case S_IFDIR:
        return "directory";
    case S_IFIFO:
        return "FIFO";
    case S_IFLNK:
        return "link simbolico";
    case S_IFREG:
        return "file regolare";
    default:
        return "tipo non riconosciuto";
    }
}

void print_permissions_group(const char *label, mode_t modes, mode_t mask) {
    mode_t group = modes & mask;

    printf("  %-13s: %03o\n", label, group);
}

int main(int argc, char *argv[]) {
    struct stat statbuf;
    mode_t modes;

    if (argc != 2)
        exit_with_err_msg("utilizzo: %s <pathname>\n", argv[0]);

    // con `lstat` un link simbolico viene osservato come link, non attraversato
    if (lstat(argv[1], &statbuf) == -1)
        exit_with_sys_err(argv[1]);

    modes = statbuf.st_mode;

    printf("pathname: %s\n", argv[1]);
    printf("st_mode completo        : %o\n", modes);
    printf("bit del tipo (S_IFMT)   : %o\n", modes & S_IFMT);
    printf("tipo riconosciuto       : %s\n", get_type_name(modes));

    printf("\nmaschere sui permessi:\n");
    print_permissions_group("S_IRWXU", modes, S_IRWXU);
    print_permissions_group("S_IRWXG", modes, S_IRWXG);
    print_permissions_group("S_IRWXO", modes, S_IRWXO);
    printf("  tutti        : %03o\n", modes & (S_IRWXU | S_IRWXG | S_IRWXO));

    exit(EXIT_SUCCESS);
}
