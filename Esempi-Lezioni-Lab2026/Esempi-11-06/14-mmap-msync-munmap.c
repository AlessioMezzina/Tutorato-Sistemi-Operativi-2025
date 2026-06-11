/**
 * modifica un file tramite una mappatura `MAP_SHARED`, sincronizza la modifica
 * con `msync` e poi rimuove la mappatura con `munmap`
 *
 * > gcc -std=c99 Esempi-11-06/14-mmap-msync-munmap.c examples/lib-misc.c -o Esempi-11-06/14-mmap-msync-munmap
 * > ./Esempi-11-06/14-mmap-msync-munmap
 */

#include "../examples/lib-misc.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define FILENAME "/tmp/oslab-msync-demo.txt"
#define INITIAL_TEXT "abcdef\n"

void create_demo_file(void) {
    int fd;

    if ((fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1)
        exit_with_sys_err(FILENAME);
    if (write(fd, INITIAL_TEXT, strlen(INITIAL_TEXT)) == -1)
        exit_with_sys_err("write");
    close(fd);
}

void print_file_content(const char *label) {
    int fd;
    char buffer[BUFSIZ];
    ssize_t size;

    if ((fd = open(FILENAME, O_RDONLY)) == -1)
        exit_with_sys_err(FILENAME);
    if ((size = read(fd, buffer, sizeof(buffer) - 1)) == -1)
        exit_with_sys_err("read");
    buffer[size] = '\0';
    close(fd);

    printf("[main] %-24s: %s", label, buffer);
}

int main(void) {
    int fd;
    char *data;
    size_t len = strlen(INITIAL_TEXT);

    create_demo_file();
    print_file_content("file iniziale");

    if ((fd = open(FILENAME, O_RDWR)) == -1)
        exit_with_sys_err(FILENAME);

    data = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED)
        exit_with_sys_err("mmap");

    printf("[main] indirizzo mappato: %p\n", (void *)data);
    printf("[main] modifico data[0] da '%c' a 'M'\n", data[0]);
    data[0] = 'M';

    printf("[main] sincronizzo con msync(..., MS_SYNC)...\n");
    if (msync(data, len, MS_SYNC) == -1)
        exit_with_sys_err("msync");
    print_file_content("dopo msync");

    printf("[main] rimuovo la mappatura con munmap...\n");
    if (munmap(data, len) == -1)
        exit_with_sys_err("munmap");

    close(fd);
    print_file_content("dopo munmap");

    if (unlink(FILENAME) == -1)
        exit_with_sys_err("unlink");

    exit(EXIT_SUCCESS);
}