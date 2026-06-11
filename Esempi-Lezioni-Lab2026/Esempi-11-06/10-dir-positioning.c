/**
 * mostra il posizionamento su un directory stream usando `telldir`,
 * `seekdir`, `rewinddir` e la chiusura finale con `closedir`
 *
 * > gcc -std=c99 Esempi-11-06/10-dir-positioning.c examples/lib-misc.c -o Esempi-11-06/10-dir-positioning
 * > ./Esempi-11-06/10-dir-positioning
 */

#include "../examples/lib-misc.h"
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DIR_TEMPLATE "/tmp/oslab-dir-positioning"

void create_empty_file(const char *pathname) {
    int fd;

    if ((fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1)
        exit_with_sys_err(pathname);
    close(fd);
}

void make_path(char *buffer, size_t size, const char *dir, const char *name) {
    snprintf(buffer, size, "%s/%s", dir, name);
}

struct dirent *read_entry(DIR *dir, const char *label) {
    struct dirent *entry;

    if ((entry = readdir(dir)) == NULL)
        exit_with_err_msg("%s: fine directory inattesa\n", label);

    printf("[main] %s: nome='%s' i-node=%lu cookie-successivo=%ld\n", label,
           entry->d_name, (unsigned long)entry->d_ino, telldir(dir));

    return entry;
}

int main(void) {
    char dirname[BUFSIZ], pathname[BUFSIZ];
    DIR *dir;
    long saved_position;
    struct dirent *entry;
    char saved_name[BUFSIZ];

    snprintf(dirname, sizeof(dirname), "%s-%d", DIR_TEMPLATE, getpid());

    printf("[main] creo una directory temporanea '%s'\n", dirname);
    if (mkdir(dirname, 0700) == -1)
        exit_with_sys_err("mkdir");

    make_path(pathname, sizeof(pathname), dirname, "uno.txt");
    create_empty_file(pathname);
    make_path(pathname, sizeof(pathname), dirname, "due.txt");
    create_empty_file(pathname);
    make_path(pathname, sizeof(pathname), dirname, "tre.txt");
    create_empty_file(pathname);

    if ((dir = opendir(dirname)) == NULL)
        exit_with_sys_err("opendir");

    read_entry(dir, "prima lettura");

    saved_position = telldir(dir);
    printf("[main] salvo la posizione corrente: cookie=%ld\n", saved_position);

    entry = read_entry(dir, "lettura successiva");
    snprintf(saved_name, sizeof(saved_name), "%s", entry->d_name);

    read_entry(dir, "altra lettura");

    printf("[main] torno alla posizione salvata con seekdir...\n");
    seekdir(dir, saved_position);
    entry = read_entry(dir, "lettura dopo seekdir");
    printf("[main] voce attesa dopo seekdir: '%s'\n", saved_name);

    printf("[main] torno all'inizio con rewinddir...\n");
    rewinddir(dir);
    read_entry(dir, "prima lettura dopo rewinddir");

    printf("[main] chiudo lo stream con closedir...\n");
    if (closedir(dir) == -1)
        exit_with_sys_err("closedir");

    make_path(pathname, sizeof(pathname), dirname, "uno.txt");
    unlink(pathname);
    make_path(pathname, sizeof(pathname), dirname, "due.txt");
    unlink(pathname);
    make_path(pathname, sizeof(pathname), dirname, "tre.txt");
    unlink(pathname);
    if (rmdir(dirname) == -1)
        exit_with_sys_err("rmdir");

    exit(EXIT_SUCCESS);
}
