/**
 * mostra l'uso elementare di `mkdir`, `rmdir`, `chdir` e `getcwd`
 *
 * > gcc -std=c99 Esempi-11-06/07-directory-basics.c examples/lib-misc.c -o Esempi-11-06/07-directory-basics
 * > ./Esempi-11-06/07-directory-basics
 */

#include "../examples/lib-misc.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DIR_TEMPLATE "/tmp/oslab-demo-dir"
#define TEST_FILE "file-interno.txt"

void print_cwd(const char *label) {
    char cwd[BUFSIZ];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
        exit_with_sys_err("getcwd");

    printf("[main] %s: %s\n", label, cwd);
}

int main(void) {
    char dirname[BUFSIZ];
    struct stat statbuf;
    int fd;

    snprintf(dirname, sizeof(dirname), "%s-%d", DIR_TEMPLATE, getpid());

    print_cwd("directory corrente iniziale");

    printf("[main] creo la directory '%s' con mode 0777...\n", dirname);
    if (mkdir(dirname, 0777) == -1)
        exit_with_sys_err("mkdir");
    if (stat(dirname, &statbuf) == -1)
        exit_with_sys_err("stat");
    printf("[main] permessi effettivi dopo umask: %03o\n",
           statbuf.st_mode & 0777);

    printf("[main] entro nella directory appena creata...\n");
    if (chdir(dirname) == -1)
        exit_with_sys_err("chdir");
    print_cwd("directory corrente dopo chdir");

    printf("[main] creo un file dentro la directory...\n");
    if ((fd = open(TEST_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1)
        exit_with_sys_err(TEST_FILE);
    close(fd);

    printf("[main] provo a cancellare la directory non vuota...\n");
    if (rmdir(dirname) == -1)
        printf("[main] rmdir fallisce come previsto: %s\n", strerror(errno));
    else
        exit_with_err_msg("anomalia: rmdir riuscita su directory non vuota\n");

    printf("[main] rimuovo il file interno e torno alla directory precedente...\n");
    if (unlink(TEST_FILE) == -1)
        exit_with_sys_err("unlink");
    if (chdir("..") == -1)
        exit_with_sys_err("chdir");
    print_cwd("directory corrente finale");

    printf("[main] cancello la directory ora vuota...\n");
    if (rmdir(dirname) == -1)
        exit_with_sys_err("rmdir");

    exit(EXIT_SUCCESS);
}
