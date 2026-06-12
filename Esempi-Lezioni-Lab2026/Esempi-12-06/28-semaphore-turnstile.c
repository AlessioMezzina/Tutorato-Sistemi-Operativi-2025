/**
 * usa un semaforo inizializzato a zero come tornello: i thread si bloccano su
 * `sem_wait` e il main li sblocca uno alla volta con `sem_post`
 *
 * > gcc -std=c99 Esempi-12-06/28-semaphore-turnstile.c examples/lib-misc.c -lpthread -o Esempi-12-06/28-semaphore-turnstile
 * > ./Esempi-12-06/28-semaphore-turnstile
 */

#include "../examples/lib-misc.h"
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 3
#define PAUSE 1

struct shared_data {
    sem_t turnstile;
};

struct thread_data {
    // variabili "private" di un thread
    pthread_t tid;
    unsigned int id;

    //puntatore alla struttura condivisa (tra tutti i thread)
    struct shared_data *shared;
};

void *thread_function(void *arg) {
    struct thread_data *data_ptr = (struct thread_data *)arg;

    printf("[T%u] attendo sul semaforo...\n", data_ptr->id);
    if (sem_wait(&data_ptr->shared->turnstile) == -1)
        exit_with_sys_err("sem_wait");

    printf("[T%u] superato il tornello\n", data_ptr->id);

    return NULL;
}

int main(void) {
    int err;
    struct shared_data shared; // qui dentro risiede il semaforo condiviso
    struct thread_data threads[NUM_THREADS];

    if (sem_init(&shared.turnstile, 0, 0) == -1)
        exit_with_sys_err("sem_init");

    errno = 0;
    if (sem_trywait(&shared.turnstile) == -1)
        printf("[main] sem_trywait su semaforo a zero fallisce: errno=%d\n",
               errno);

    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        threads[i].id = i + 1;
        threads[i].shared = &shared;

        if ((err = pthread_create(&threads[i].tid, NULL, thread_function,
                                  (void *)&threads[i])) != 0)
            exit_with_err("pthread_create", err);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        sleep(PAUSE);
        printf("[main] sem_post: lascio passare un thread\n");
        if (sem_post(&shared.turnstile) == -1)
            exit_with_sys_err("sem_post");
    }

    for (int i = 0; i < NUM_THREADS; i++)
        if ((err = pthread_join(threads[i].tid, NULL)) != 0)
            exit_with_err("pthread_join", err);

    if (sem_destroy(&shared.turnstile) == -1)
        exit_with_sys_err("sem_destroy");

    exit(EXIT_SUCCESS);
}
