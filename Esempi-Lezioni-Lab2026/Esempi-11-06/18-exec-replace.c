/**
 * mostra l'uso base di `exec`: il figlio viene creato con `fork` e poi
 * sostituisce il proprio programma con quello indicato tramite `execlp` o
 * `execvp`; se `exec` riesce, il codice successivo non viene eseguito
 *
 * > gcc -std=c99 Esempi-12-06/03-exec-replace.c examples/lib-misc.c -o Esempi-12-06/03-exec-replace
 * > ./Esempi-12-06/03-exec-replace
 */

#include "../examples/lib-misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void print_exit_status(const char *name, int status) {
    if (WIFEXITED(status)) {
        printf("[P] %s terminato con exit-code %d\n", name,
               WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[P] %s terminato da segnale %d\n", name, WTERMSIG(status));
    }
}

void run_execlp_demo(void) {
    printf("[F1] prima di execlp: pid=%d\n", getpid());
    execlp("echo", "echo", "[F1] testo prodotto dal programma echo", (char *)0);

    fprintf(stderr,
            "[F1] questa riga viene eseguita solo se execlp fallisce\n");
    exit_with_sys_err("execlp");
}

void run_execvp_demo(void) {
    char *args[] = {"echo", "[F2] argomenti passati tramite vettore", NULL};

    printf("[F2] prima di execvp: pid=%d\n", getpid());
    execvp(args[0], args);

    fprintf(stderr,
            "[F2] questa riga viene eseguita solo se execvp fallisce\n");
    exit_with_sys_err("execvp");
}

void run_exec_error_demo(void) {
    printf("[F3] provo ad eseguire un comando inesistente\n");
    execlp("comando-che-non-esiste", "comando-che-non-esiste", (char *)0);

    fprintf(stderr, "[F3] exec e' fallita: controllo errno ed esco\n");
    exit_with_sys_err("execlp");
}

void create_child(const char *name, void (*child_function)(void)) {
    pid_t pid;
    int status;

    printf("[P] creo %s...\n", name);
    if ((pid = fork()) == -1)
        exit_with_sys_err("fork");

    if (pid == 0)
        child_function();

    if (waitpid(pid, &status, 0) == -1)
        exit_with_sys_err("waitpid");
    print_exit_status(name, status);
}

int main(void) {
    setbuf(stdout, NULL);
    printf("[P] padre attivato con pid %d\n", getpid());

    create_child("F1/execlp", run_execlp_demo);
    create_child("F2/execvp", run_execvp_demo);
    create_child("F3/errore", run_exec_error_demo);

    printf("[P] fine dimostrazione\n");

    exit(EXIT_SUCCESS);
}
