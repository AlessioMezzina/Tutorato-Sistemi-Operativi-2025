/**
 * confronta `MAP_PRIVATE` e `MAP_SHARED` mappando lo stesso file in memoria
 *
 * > gcc -std=c99 Esempi-11-06/13-mmap-shared-private.c examples/lib-misc.c -o Esempi-11-06/13-mmap-shared-private
 * > ./Esempi-11-06/13-mmap-shared-private
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

#define FILENAME "/tmp/oslab-mmap-demo.txt"
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

    printf("[main] %-26s: %s", label, buffer);
}

void map_and_modify(int flags, char replacement) {
    int fd;
    char *data;
    size_t len = strlen(INITIAL_TEXT);

    if ((fd = open(FILENAME, O_RDWR)) == -1)
        exit_with_sys_err(FILENAME);

    data = mmap(NULL, len, PROT_READ | PROT_WRITE, flags, fd, 0);
    if (data == MAP_FAILED)
        exit_with_sys_err("mmap");

    printf("[main] contenuto nella mappa: %.*s", (int)len, data);
    data[0] = replacement;
    printf("[main] modifico data[0] = '%c'\n", replacement);

    if (munmap(data, len) == -1)
        exit_with_sys_err("munmap");
    close(fd);
}

int main(void) {
    create_demo_file();
    print_file_content("file iniziale");

    printf("\n[main] mappatura MAP_PRIVATE...\n");
    map_and_modify(MAP_PRIVATE, 'P');
    print_file_content("dopo MAP_PRIVATE");

    printf("\n[main] mappatura MAP_SHARED...\n");
    map_and_modify(MAP_SHARED, 'S');
    print_file_content("dopo MAP_SHARED");

    if (unlink(FILENAME) == -1)
        exit_with_sys_err("unlink");

    exit(EXIT_SUCCESS);
}
