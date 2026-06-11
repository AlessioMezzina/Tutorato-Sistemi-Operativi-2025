/**
 * mostra due modi di usare il parametro `pid` di `kill`: con un valore positivo
 * si segnala un singolo processo, con un valore negativo si segnala un gruppo
 * di processi
 *
 * > gcc -std=c99 Esempi-12-06/07-kill-process-group.c examples/lib-misc.c -o Esempi-12-06/07-kill-process-group
 * > ./Esempi-12-06/07-kill-process-group
 */

#include "../examples/lib-misc.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PAUSE 1

void handle_sigusr1(int signo) {
    const char message[] = "[handler] ricevuto SIGUSR1\n";

    (void)signo;
    write(STDOUT_FILENO, message, sizeof(message) - 1);
}

void child_function(const char *name, pid_t pgid) {
    if (setpgid(0, pgid) == -1)
        exit_with_sys_err("setpgid");
    if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR)
        exit_with_sys_err("signal");

    printf("[%s] pid=%d pgid=%d: in attesa di segnali\n", name, getpid(),
           getpgrp());

    while (1) {
        pause();
        printf("[%s] pause interrotta da un segnale gestito\n", name);
    }
}

void create_leader(pid_t *leader_pid) {
    if ((*leader_pid = fork()) == -1)
        exit_with_sys_err("fork");

    if (*leader_pid == 0)
        child_function("F1", 0);

    if (setpgid(*leader_pid, *leader_pid) == -1)
        exit_with_sys_err("setpgid");
}

void create_member(pid_t leader_pid, pid_t *member_pid) {
    if ((*member_pid = fork()) == -1)
        exit_with_sys_err("fork");

    if (*member_pid == 0)
        child_function("F2", leader_pid);

    if (setpgid(*member_pid, leader_pid) == -1)
        exit_with_sys_err("setpgid");
}

void wait_child(pid_t pid) {
    int status;

    if (waitpid(pid, &status, 0) == -1)
        exit_with_sys_err("waitpid");
    if (WIFSIGNALED(status)) {
        printf("[P] figlio %d terminato dal segnale %d\n", pid,
               WTERMSIG(status));
    }
}

int main(void) {
    pid_t leader_pid, member_pid;

    setbuf(stdout, NULL);

    create_leader(&leader_pid);
    create_member(leader_pid, &member_pid);

    sleep(PAUSE);
    printf("[P] F1=%d e F2=%d sono nel gruppo %d\n", leader_pid, member_pid,
           leader_pid);

    printf("[P] kill(%d, SIGUSR1): segnalo solo F1\n", leader_pid);
    if (kill(leader_pid, SIGUSR1) == -1)
        exit_with_sys_err("kill");

    sleep(PAUSE);
    printf("[P] kill(-%d, SIGUSR1): segnalo tutto il gruppo\n", leader_pid);
    if (kill(-leader_pid, SIGUSR1) == -1)
        exit_with_sys_err("kill");

    sleep(PAUSE);
    printf("[P] termino il gruppo con kill(-%d, SIGTERM)\n", leader_pid);
    if (kill(-leader_pid, SIGTERM) == -1)
        exit_with_sys_err("kill");

    wait_child(leader_pid);
    wait_child(member_pid);

    exit(EXIT_SUCCESS);
}
