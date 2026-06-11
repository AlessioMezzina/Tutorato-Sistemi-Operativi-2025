/**
 * verifica una condizione costruita su `st_mode`: l'oggetto non deve essere
 * una directory e il proprietario deve avere solo il permesso di esecuzione
 *
 * > gcc -std=c99 Esempi-11-06/05-stat-mode-condition.c examples/lib-misc.c -o Esempi-11-06/05-stat-mode-condition
 * > cp Esempi-11-06/file.txt /tmp/solo-exec.txt
 * > chmod 100 /tmp/solo-exec.txt
 * > ./Esempi-11-06/05-stat-mode-condition /tmp/solo-exec.txt
 * > ./Esempi-11-06/05-stat-mode-condition Esempi-11-06
 */

#include "../examples/lib-misc.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

void print_bool(const char *label, bool value) {
    printf("  %-45s: %s\n", label, value ? "vero" : "falso");
}

int main(int argc, char *argv[]) {
    struct stat statbuf;
    mode_t owner_permissions;
    bool is_not_dir, owner_only_execute, condition;

    if (argc != 2)
        exit_with_err_msg("utilizzo: %s <pathname>\n", argv[0]);

    if (stat(argv[1], &statbuf) == -1)
        exit_with_sys_err(argv[1]);

    owner_permissions = statbuf.st_mode & S_IRWXU; // maschera per i permessi del proprietario
    // S_IRWXU = 00700, quindi owner_permissions conterrà solo i bit relativi ai permessi del proprietario
    is_not_dir = !S_ISDIR(statbuf.st_mode);
    owner_only_execute = (owner_permissions == S_IXUSR);
    condition = is_not_dir && owner_only_execute;

    printf("pathname: %s\n", argv[1]);
    printf("st_mode completo             : %o\n", statbuf.st_mode);
    printf("permessi del proprietario    : %03o\n", owner_permissions);
    printf("permesso atteso del propriet.: %03o\n\n", S_IXUSR);

    print_bool("non e' una directory", is_not_dir);
    print_bool("proprietario: solo esecuzione", owner_only_execute);
    print_bool("condizione complessiva", condition);

    exit(condition ? EXIT_SUCCESS : EXIT_FAILURE);
}
