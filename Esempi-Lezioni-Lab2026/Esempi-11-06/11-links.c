/**
 * mostra la differenza tra hard link e symbolic link e usa `readlink`,
 * `rename`, `unlink` e `remove`
 *
 * > gcc -std=c99 Esempi-11-06/11-links.c examples/lib-misc.c -o Esempi-11-06/11-links
 * > ./Esempi-11-06/11-links
 */

#include "../examples/lib-misc.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DIR_TEMPLATE "/tmp/oslab-links"

void make_path(char *buffer, size_t size, const char *dir, const char *name) {
    snprintf(buffer, size, "%s/%s", dir, name);
}

void create_file(const char *pathname) {
    int fd;
    const char text[] = "contenuto del file dimostrativo\n";

    if ((fd = open(pathname, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1)
        exit_with_sys_err(pathname);
    if (write(fd, text, strlen(text)) == -1)
        exit_with_sys_err("write");
    close(fd);
}

void print_stat_summary(const char *label, const char *pathname) {
    struct stat statbuf;
    const char *type;

    if (lstat(pathname, &statbuf) == -1)
        exit_with_sys_err(pathname);

    if (S_ISREG(statbuf.st_mode))
        type = "file regolare";
    else if (S_ISLNK(statbuf.st_mode))
        type = "link simbolico";
    else if (S_ISDIR(statbuf.st_mode))
        type = "directory";
    else
        type = "altro";

    printf("[main] %-18s path='%s'\n", label, pathname);
    printf("       tipo=%s i-node=%lu st_nlink=%lu size=%lld\n", type,
           (unsigned long)statbuf.st_ino, (unsigned long)statbuf.st_nlink,
           (long long)statbuf.st_size);
}

int main(void) {
    char dirname[BUFSIZ];
    char original[BUFSIZ], hardlink[BUFSIZ], renamed[BUFSIZ], symlink_path[BUFSIZ];
    char link_target[BUFSIZ];
    ssize_t size;

    snprintf(dirname, sizeof(dirname), "%s-%d", DIR_TEMPLATE, getpid());
    make_path(original, sizeof(original), dirname, "originale.txt");
    make_path(hardlink, sizeof(hardlink), dirname, "hard-link.txt");
    make_path(renamed, sizeof(renamed), dirname, "hard-link-rinominato.txt");
    make_path(symlink_path, sizeof(symlink_path), dirname, "symbolic-link.txt");

    printf("[main] creo directory temporanea '%s'\n", dirname);
    if (mkdir(dirname, 0700) == -1)
        exit_with_sys_err("mkdir");

    printf("[main] creo il file originale...\n");
    create_file(original);
    print_stat_summary("originale", original);

    printf("\n[main] creo un hard link allo stesso file...\n");
    if (link(original, hardlink) == -1)
        exit_with_sys_err("link");
    print_stat_summary("originale", original);
    print_stat_summary("hard link", hardlink);

    printf("\n[main] creo un link simbolico verso il file originale...\n");
    if (symlink(original, symlink_path) == -1)
        exit_with_sys_err("symlink");
    print_stat_summary("symbolic link", symlink_path);

    size = readlink(symlink_path, link_target, sizeof(link_target) - 1);
    if (size == -1)
        exit_with_sys_err("readlink");
    link_target[size] = '\0'; // readlink non aggiunge il terminatore nullo
    printf("[main] readlink('%s') -> '%s'\n", symlink_path, link_target);

    printf("\n[main] rinomino l'hard link...\n");
    if (rename(hardlink, renamed) == -1)
        exit_with_sys_err("rename");
    print_stat_summary("hard link rin.", renamed);

    printf("\n[main] rimuovo i nomi creati...\n");
    if (unlink(symlink_path) == -1)
        exit_with_sys_err("unlink symlink");
    if (remove(renamed) == -1)
        exit_with_sys_err("remove hard link");
    print_stat_summary("originale rimasto", original);
    if (unlink(original) == -1)
        exit_with_sys_err("unlink originale");
    if (rmdir(dirname) == -1)
        exit_with_sys_err("rmdir");

    exit(EXIT_SUCCESS);
}
