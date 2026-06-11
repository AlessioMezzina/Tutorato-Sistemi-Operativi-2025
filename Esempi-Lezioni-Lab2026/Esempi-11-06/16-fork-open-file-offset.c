/**
 * mostra che dopo `fork` padre e figlio hanno descrittori separati, ma
 * condividono la stessa voce nella tabella globale dei file aperti: il figlio
 * fa avanzare il file offset e modifica un flag di stato, il padre vede
 * entrambi gli effetti
 *
 * > gcc -std=c99 Esempi-12-06/01-fork-open-file-offset.c examples/lib-misc.c -o Esempi-12-06/01-fork-open-file-offset
 * > ./Esempi-12-06/01-fork-open-file-offset
 */

#include "../examples/lib-misc.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FILENAME "/tmp/oslab-fork-offset.txt"
#define FILE_CONTENT "0123456789\n"
#define READ_SIZE 4
#define PAUSE 1

off_t current_offset(int fd) {
    off_t offset;

    if ((offset = lseek(fd, 0, SEEK_CUR)) == -1)
        exit_with_sys_err("lseek");

    return offset;
}

void print_append_flag(const char *process_name, int fd) {
    int flags;

    if ((flags = fcntl(fd, F_GETFL)) == -1)
        exit_with_sys_err("fcntl");

    printf("[%s] flag O_APPEND: %s\n", process_name,
           (flags & O_APPEND) ? "attivo" : "non attivo");
}

void read_chunk(const char *process_name, int fd) {
    char buffer[READ_SIZE + 1];
    ssize_t nread;

    if ((nread = read(fd, buffer, READ_SIZE)) == -1)
        exit_with_sys_err("read");
    buffer[nread] = '\0';

    printf("[%s] read: \"%s\"; offset=%lld\n", process_name, buffer,
           (long long)current_offset(fd));
}

void create_demo_file(void) {
    int fd;

    if ((fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1)
        exit_with_sys_err(FILENAME);
    if (write(fd, FILE_CONTENT, strlen(FILE_CONTENT)) !=
        (ssize_t)strlen(FILE_CONTENT))
        exit_with_sys_err("write");
    close(fd);
}

int main(void) {
    int fd, flags;
    pid_t pid;

    setbuf(stdout, NULL);
    create_demo_file();

    if ((fd = open(FILENAME, O_RDWR)) == -1)
        exit_with_sys_err(FILENAME);

    printf("[P] file aperto prima della fork: fd=%d, offset=%lld\n", fd,
           (long long)current_offset(fd));
    print_append_flag("P", fd);

    printf("[P] fork...\n");
    if ((pid = fork()) == -1)
        exit_with_sys_err("fork");

    if (pid == 0) {
        // il figlio usa il descrittore ereditato dal padre
        printf("[F] eredito fd=%d, offset=%lld\n", fd,
               (long long)current_offset(fd));
        read_chunk("F", fd);

        if ((flags = fcntl(fd, F_GETFL)) == -1)
            exit_with_sys_err("fcntl");
        if (fcntl(fd, F_SETFL, flags | O_APPEND) == -1)
            exit_with_sys_err("fcntl");

        printf("[F] attivo O_APPEND e chiudo il mio descrittore\n");
        print_append_flag("F", fd);
        close(fd);
        exit(EXIT_SUCCESS);
    }

    // il padre aspetta solo per rendere l'esecuzione leggibile
    sleep(PAUSE);
    printf("[P] dopo il figlio: offset=%lld\n", (long long)current_offset(fd));
    print_append_flag("P", fd);
    read_chunk("P", fd);

    close(fd);
    if (unlink(FILENAME) == -1)
        exit_with_sys_err("unlink");

    exit(EXIT_SUCCESS);
}
