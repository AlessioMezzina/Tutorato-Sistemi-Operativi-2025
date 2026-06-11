/**
 * coordinamento semplice tra padre e figli con `waitpid` e `wait`: il padre
 * aspetta prima un figlio specifico e poi raccoglie un qualunque figlio rimasto
 *
 * > gcc -std=c99 Esempi-12-06/02-wait-waitpid.c examples/lib-misc.c -o Esempi-12-06/02-wait-waitpid
 * > ./Esempi-12-06/02-wait-waitpid
 */

#include "../examples/lib-misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FAST_PAUSE 1
#define SLOW_PAUSE 2

void child_function(const char *name, int pause, int exit_code) {
    printf("[%s] pid=%d ppid=%d\n", name, getpid(), getppid());
    printf("[%s] pausa di %d %s...\n", name, pause,
           pause == 1 ? "secondo" : "secondi");
    sleep(pause);

    printf("[%s] terminazione con exit-code %d\n", name, exit_code);
    exit(exit_code);
}

void print_exit_status(pid_t pid, int status) {
    if (WIFEXITED(status)) {
        printf("[P] figlio con pid %d terminato normalmente, exit-code=%d\n",
               pid, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[P] figlio con pid %d terminato da segnale %d\n", pid,
               WTERMSIG(status));
    } else {
        printf("[P] figlio con pid %d terminato con stato non gestito\n", pid);
    }
}

int main(void) {
    pid_t f1_pid, f2_pid, pid;
    int status;

    setbuf(stdout, NULL);
    printf("[P] padre attivato con pid %d\n", getpid());

    if ((f1_pid = fork()) == -1)
        exit_with_sys_err("fork");
    if (f1_pid == 0)
        child_function("F1", SLOW_PAUSE, 7);

    if ((f2_pid = fork()) == -1)
        exit_with_sys_err("fork");
    if (f2_pid == 0)
        child_function("F2", FAST_PAUSE, 3);

    printf("[P] creati F1=%d e F2=%d\n", f1_pid, f2_pid);
    printf("[P] attendo specificamente F1 con waitpid(%d, ..., 0)\n",
           f1_pid);

    if ((pid = waitpid(f1_pid, &status, 0)) == -1)
        exit_with_sys_err("waitpid");
    print_exit_status(pid, status);

    printf("[P] ora raccolgo un qualunque figlio rimasto con wait(...)\n");
    if ((pid = wait(&status)) == -1)
        exit_with_sys_err("wait");
    print_exit_status(pid, status);

    printf("[P] tutti gli exit status sono stati raccolti\n");

    exit(EXIT_SUCCESS);
}
