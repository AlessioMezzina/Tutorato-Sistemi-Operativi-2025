/**
 * mostra alcune operazioni varie su file: `truncate`, `ftruncate`, `chmod` e
 * una chiamata innocua a `chown`
 *
 * > gcc -std=c99 Esempi-11-06/12-file-operations.c examples/lib-misc.c -o Esempi-11-06/12-file-operations
 * > ./Esempi-11-06/12-file-operations
 */

#include "../examples/lib-misc.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FILENAME "/tmp/oslab-file-operations.txt"

void print_file_info(const char *label, const char *pathname) {
    struct stat statbuf;

    if (stat(pathname, &statbuf) == -1)
        exit_with_sys_err(pathname);

    printf("[main] %-24s size=%lld permessi=%03o uid=%u gid=%u\n", label,
           (long long)statbuf.st_size, statbuf.st_mode & 0777,
           (unsigned int)statbuf.st_uid, (unsigned int)statbuf.st_gid);
}

int main(void) {
    int fd;
    const char text[] = "abcdefghijklmnopqrstuvwxyz\n";

    if ((fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1)
        exit_with_sys_err(FILENAME);
    if (write(fd, text, strlen(text)) == -1)
        exit_with_sys_err("write");
    close(fd);

    print_file_info("file appena creato", FILENAME);

    printf("[main] accorcio il file con truncate a 10 byte...\n");
    if (truncate(FILENAME, 10) == -1)
        exit_with_sys_err("truncate");
    print_file_info("dopo truncate", FILENAME);

    printf("[main] riapro e allungo il file con ftruncate a 4096 byte...\n");
    if ((fd = open(FILENAME, O_RDWR)) == -1)
        exit_with_sys_err(FILENAME);
    if (ftruncate(fd, 4096) == -1)
        exit_with_sys_err("ftruncate");
    close(fd);
    print_file_info("dopo ftruncate", FILENAME);

    printf("[main] cambio i permessi con chmod a 0644...\n");
    if (chmod(FILENAME, 0644) == -1)
        exit_with_sys_err("chmod");
    print_file_info("dopo chmod", FILENAME);

    printf("[main] chown(path, -1, -1): non cambia owner/gruppo...\n");
    if (chown(FILENAME, (uid_t)-1, (gid_t)-1) == -1)
        exit_with_sys_err("chown");
    print_file_info("dopo chown no-op", FILENAME);

    if (unlink(FILENAME) == -1)
        exit_with_sys_err("unlink");

    exit(EXIT_SUCCESS);
}
