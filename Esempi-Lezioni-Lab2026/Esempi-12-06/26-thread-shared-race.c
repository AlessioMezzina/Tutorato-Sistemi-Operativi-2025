/**
 * mostra una race condition su un contatore condiviso tra thread: i dati sono
 * passati tramite strutture, non tramite variabili globali, ma manca la
 * sincronizzazione sulla sezione critica
 *
 * > gcc -std=c99 Esempi-12-06/26-thread-shared-race.c examples/lib-misc.c -lpthread -o Esempi-12-06/26-thread-shared-race
 * > ./Esempi-12-06/26-thread-shared-race
 */

#include "../examples/lib-misc.h"
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 6
#define INCREMENTS_PER_THREAD 200000

struct shared_data {
    unsigned long counter;
};

struct thread_data {
    //variabili private per ogni thread
    pthread_t tid;
    unsigned int id;
    unsigned long increments;

    //variabili condivise tra thread
    struct shared_data *shared;
};

void *thread_function(void *arg) {
    struct thread_data *data_ptr = (struct thread_data *)arg;
    // per accedere alla variabile condivisa: data_ptr->shared->counter
    printf("[T%u] incremento...\n", data_ptr->id);

    while (data_ptr->increments > 0) {
        unsigned long value = data_ptr->shared->counter;

        if ((data_ptr->increments % 1000) == 0)
            sched_yield();

        data_ptr->shared->counter = value + 1;
        data_ptr->increments--;
    }

    return NULL;
}

int main(void) {
    int err;
    struct shared_data shared = {0};
    struct thread_data threads[NUM_THREADS];
    unsigned long expected = NUM_THREADS * INCREMENTS_PER_THREAD;

    for (unsigned int i = 0; i < NUM_THREADS; i++) {
        threads[i].id = i + 1;
        threads[i].increments = INCREMENTS_PER_THREAD;
        threads[i].shared = &shared;

        if ((err = pthread_create(&threads[i].tid, NULL, thread_function,
                                  (void *)&threads[i])) != 0)
            exit_with_err("pthread_create", err);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        if ((err = pthread_join(threads[i].tid, NULL)) != 0)
            exit_with_err("pthread_join", err);

    printf("[main] valore finale: reale=%lu atteso=%lu\n", shared.counter,
           expected);
    if (shared.counter != expected)
        printf("[main] race condition osservata: alcuni incrementi sono persi\n");
    else
        printf("[main] risultato corretto per caso: la race resta presente\n");

    exit(EXIT_SUCCESS);
}
