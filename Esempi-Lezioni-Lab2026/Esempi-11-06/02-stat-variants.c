/**
 * confronta `stat`, `fstat` e `lstat` su un oggetto del file-system,
 * visualizzando alcuni campi principali della struttura `stat`
 *
 * > gcc -std=c99 Esempi-11-06/02-stat-variants.c examples/lib-misc.c -o Esempi-11-06/02-stat-variants
 * > ./Esempi-11-06/02-stat-variants Esempi-11-06/file.txt
 * > ln -sf file.txt Esempi-11-06/link-file.txt
 * > ./Esempi-11-06/02-stat-variants Esempi-11-06/link-file.txt
 */

#include "../examples/lib-misc.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

void print_time(const char *name, time_t value) {
    printf("  %s: %s", name, asctime(localtime(&value)));
}

void print_stat_info(const char *label, struct stat *statbuf) {
    printf("\n[%s]\n", label);
    printf("  st_mode : %o\n", statbuf->st_mode);
    printf("  st_uid  : %u\n", statbuf->st_uid);
    printf("  st_gid  : %u\n", statbuf->st_gid);
    printf("  st_ino  : %lu\n", (unsigned long)statbuf->st_ino);
    printf("  st_nlink: %lu\n", (unsigned long)statbuf->st_nlink);
    printf("  st_size : %lld byte\n", (long long)statbuf->st_size);
    print_time("st_atime", statbuf->st_atime);
    print_time("st_mtime", statbuf->st_mtime);
    print_time("st_ctime", statbuf->st_ctime);
}

int main(int argc, char *argv[]) {
    int fd;
    struct stat statbuf;

    if (argc != 2)
        exit_with_err_msg("utilizzo: %s <pathname>\n", argv[0]);

    printf("[main] oggetto da ispezionare: %s\n", argv[1]);

    // `stat` parte da pathname e attraversa eventuali link simbolici
    if (stat(argv[1], &statbuf) == -1)
        exit_with_sys_err("stat");
    print_stat_info("stat(pathname)", &statbuf);

    // `lstat` parte da pathname ma non attraversa eventuali link simbolici
    if (lstat(argv[1], &statbuf) == -1)
        exit_with_sys_err("lstat");
    print_stat_info("lstat(pathname)", &statbuf);

    // `fstat` parte da un file descriptor gia' aperto
    if ((fd = open(argv[1], O_RDONLY)) == -1)
        exit_with_sys_err(argv[1]);
    if (fstat(fd, &statbuf) == -1)
        exit_with_sys_err("fstat");
    print_stat_info("fstat(fd)", &statbuf);

    close(fd);

    exit(EXIT_SUCCESS);
}
