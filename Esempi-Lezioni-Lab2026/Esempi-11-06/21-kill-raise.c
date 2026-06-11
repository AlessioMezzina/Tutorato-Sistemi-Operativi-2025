/**
 * mostra la differenza tra `kill`, che invia un segnale ad un processo indicato
 * da PID, e `raise`, che invia un segnale al processo chiamante stesso
 *
 * > gcc -std=c99 Esempi-12-06/06-kill-raise.c examples/lib-misc.c -o Esempi-12-06/06-kill-raise
 * > ./Esempi-12-06/06-kill-raise
 */

#include "../examples/lib-misc.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PAUSE 1

void handle_signal(int signo) {
    if (signo == SIGUSR1) {
        const char message[] = "[handler] ricevuto SIGUSR1\n";
        write(STDOUT_FILENO, message, sizeof(message) - 1);
    } else if (signo == SIGUSR2) {
        const char message[] = "[handler] ricevuto SIGUSR2\n";
        write(STDOUT_FILENO, message, sizeof(message) - 1);
    }
}

void child_function(void) {
    if (signal(SIGUSR1, handle_signal) == SIG_ERR)
        exit_with_sys_err("signal");

    printf("[F] pid=%d: mi metto in attesa di SIGUSR1\n", getpid());
    pause();
    printf("[F] pause interrotta: termino\n");

    exit(EXIT_SUCCESS);
}

void print_exit_status(pid_t pid, int status) {
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

    if (signal(SIGUSR2, handle_signal) == SIG_ERR)
        exit_with_sys_err("signal");

    if ((pid = fork()) == -1)
        exit_with_sys_err("fork");
    if (pid == 0)
        child_function();

    sleep(PAUSE);
    printf("[P] invio SIGUSR1 al figlio con kill(%d, SIGUSR1)\n", pid);
    if (kill(pid, SIGUSR1) == -1)
        exit_with_sys_err("kill");

    if (waitpid(pid, &status, 0) == -1)
        exit_with_sys_err("waitpid");
    print_exit_status(pid, status);

    printf("[P] invio SIGUSR2 a me stesso con raise(SIGUSR2)\n");
    if (raise(SIGUSR2) != 0)
        exit_with_sys_err("raise");

    exit(EXIT_SUCCESS);
}
