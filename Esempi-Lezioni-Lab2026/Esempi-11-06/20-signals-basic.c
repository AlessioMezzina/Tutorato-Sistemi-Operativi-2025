/**
 * mostra tre reazioni diverse ai segnali: il figlio ignora `SIGINT`, gestisce
 * `SIGTERM` con una procedura apposita e viene infine terminato da `SIGKILL`
 *
 * > gcc -std=c99 Esempi-12-06/05-signals-basic.c examples/lib-misc.c -o Esempi-12-06/05-signals-basic
 * > ./Esempi-12-06/05-signals-basic
 */

#include "../examples/lib-misc.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PAUSE 1

void handle_sigterm(int signo) {
    const char message[] = "[F] ricevuto SIGTERM: eseguo il gestore\n";

    (void)signo;
    write(STDOUT_FILENO, message, sizeof(message) - 1);
}

void child_function(void) {
    if (signal(SIGINT, SIG_IGN) == SIG_ERR)
        exit_with_sys_err("signal");
    if (signal(SIGTERM, handle_sigterm) == SIG_ERR)
        exit_with_sys_err("signal");

    printf("[F] pid=%d: ignoro SIGINT e gestisco SIGTERM\n", getpid());

    while (1) {
        printf("[F] in attesa di segnali...\n");
        sleep(PAUSE);
    }
}

void print_child_status(pid_t pid, int status) {
    if (WIFEXITED(status)) {
        printf("[P] figlio %d terminato con exit-code %d\n", pid,
               WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[P] figlio %d terminato dal segnale %d\n", pid,
               WTERMSIG(status));
    }
}

int main(void) {
    pid_t pid;
    int status;

    setbuf(stdout, NULL);

    if ((pid = fork()) == -1)
        exit_with_sys_err("fork");
    if (pid == 0)
        child_function();

    printf("[P] figlio creato con pid %d\n", pid);
    sleep(PAUSE);

    printf("[P] invio SIGINT: il figlio lo ignora\n");
    if (kill(pid, SIGINT) == -1)
        exit_with_sys_err("kill");
    sleep(PAUSE);

    printf("[P] invio SIGTERM: il figlio esegue il gestore\n");
    if (kill(pid, SIGTERM) == -1)
        exit_with_sys_err("kill");
    sleep(PAUSE);

    printf("[P] invio SIGKILL: il figlio non puo' gestirlo\n");
    if (kill(pid, SIGKILL) == -1)
        exit_with_sys_err("kill");

    if (waitpid(pid, &status, 0) == -1)
        exit_with_sys_err("waitpid");
    print_child_status(pid, status);

    exit(EXIT_SUCCESS);
}
