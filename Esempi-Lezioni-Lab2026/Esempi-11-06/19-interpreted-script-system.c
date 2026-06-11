/**
 * crea uno script testuale con shebang, lo rende eseguibile e lo lancia con
 * `exec`; poi mostra l'uso di `system` per far eseguire una stringa alla shell
 *
 * > gcc -std=c99 Esempi-12-06/04-interpreted-script-system.c examples/lib-misc.c -o Esempi-12-06/04-interpreted-script-system
 * > ./Esempi-12-06/04-interpreted-script-system
 */

#include "../examples/lib-misc.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define SCRIPT_PATH "/tmp/oslab-interpreted-script.sh"
#define OUTPUT_PATH "/tmp/oslab-system-output.txt"

void write_file(const char *path, const char *content, mode_t mode) {
    int fd;

    if ((fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode)) == -1)
        exit_with_sys_err(path);
    if (write(fd, content, strlen(content)) != (ssize_t)strlen(content))
        exit_with_sys_err("write");
    close(fd);
}

void print_exit_status(const char *label, int status) {
    if (WIFEXITED(status)) {
        printf("[P] %s terminato con exit-code %d\n", label,
               WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[P] %s terminato da segnale %d\n", label, WTERMSIG(status));
    }
}

void print_file_content(const char *path) {
    int fd;
    char buffer[BUFSIZ];
    ssize_t nread;

    if ((fd = open(path, O_RDONLY)) == -1)
        exit_with_sys_err(path);
    if ((nread = read(fd, buffer, sizeof(buffer) - 1)) == -1)
        exit_with_sys_err("read");
    buffer[nread] = '\0';
    close(fd);

    printf("[P] contenuto di %s:\n%s", path, buffer);
}

void create_script(void) {
    const char *content =
        "#!/bin/sh\n"
        "echo \"[script] interprete: /bin/sh\"\n"
        "echo \"[script] nome script: $0\"\n"
        "echo \"[script] argomenti: $1 $2\"\n";

    write_file(SCRIPT_PATH, content, 0600);
    if (chmod(SCRIPT_PATH, 0700) == -1)
        exit_with_sys_err("chmod");

    printf("[P] script creato in %s e reso eseguibile\n", SCRIPT_PATH);
}

void run_script_with_exec(void) {
    pid_t pid;
    int status;

    printf("[P] creo un figlio che esegue lo script con exec...\n");
    if ((pid = fork()) == -1)
        exit_with_sys_err("fork");

    if (pid == 0) {
        printf("[F] exec dello script testuale...\n");
        execl(SCRIPT_PATH, SCRIPT_PATH, "primo", "secondo", (char *)0);
        exit_with_sys_err("execl");
    }

    if (waitpid(pid, &status, 0) == -1)
        exit_with_sys_err("waitpid");
    print_exit_status("script", status);
}

void run_command_with_system(void) {
    int status;
    char command[BUFSIZ];

    snprintf(command, sizeof(command),
             "echo '[system] comando eseguito dalla shell' > %s", OUTPUT_PATH);

    printf("[P] system(\"%s\")\n", command);
    if ((status = system(command)) == -1)
        exit_with_sys_err("system");

    print_exit_status("system", status);
    print_file_content(OUTPUT_PATH);
}

int main(void) {
    setbuf(stdout, NULL);

    create_script();
    run_script_with_exec();
    run_command_with_system();

    if (unlink(SCRIPT_PATH) == -1)
        exit_with_sys_err("unlink script");
    if (unlink(OUTPUT_PATH) == -1)
        exit_with_sys_err("unlink output");

    exit(EXIT_SUCCESS);
}
