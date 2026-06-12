/**
 * corregge l'incremento concorrente del contatore usando un mutex condiviso;
 * mostra anche `pthread_mutex_trylock` su un lock gia' occupato
 *
 * > gcc -std=c99 Esempi-12-06/27-thread-mutex-counter.c examples/lib-misc.c -lpthread -o Esempi-12-06/27-thread-mutex-counter
 * > ./Esempi-12-06/27-thread-mutex-counter
 */

#include "../examples/lib-misc.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 4
#define INCREMENTS_PER_THREAD 100000

struct shared_data {
    unsigned long counter;
    pthread_mutex_t lock;
};

struct thread_data {
    //variabili "private" di un thread
    pthread_t tid;
    unsigned int id;
    unsigned long increments;

    //puntatore alla struttura condivisa (tra tutti i thread)
    struct shared_data *shared;
};

void *thread_function(void *arg) {
    int err;
    struct thread_data *data_ptr = (struct thread_data *)arg;

    printf("[T%u] incremento con mutex...\n", data_ptr->id);

    while (data_ptr->increments > 0) {
        pthread_mutex_lock(&data_ptr->shared->lock);
        
        data_ptr->shared->counter++;
        
        pthread_mutex_unlock(&data_ptr->shared->lock);
        data_ptr->increments--;
    }

    return NULL;
}

int main(void) {
    int err;
    struct shared_data shared = {0, PTHREAD_MUTEX_INITIALIZER};
    struct thread_data threads[NUM_THREADS];
    unsigned long expected = NUM_THREADS * INCREMENTS_PER_THREAD;

    if ((err = pthread_mutex_lock(&shared.lock)) != 0)
        exit_with_err("pthread_mutex_lock", err);
    if ((err = pthread_mutex_trylock(&shared.lock)) == EBUSY)
        printf("[main] trylock: mutex gia' occupato, ritorno EBUSY\n");
    else if (err != 0)
        exit_with_err("pthread_mutex_trylock", err);
    if ((err = pthread_mutex_unlock(&shared.lock)) != 0)
        exit_with_err("pthread_mutex_unlock", err);

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

    if ((err = pthread_mutex_destroy(&shared.lock)) != 0)
        exit_with_err("pthread_mutex_destroy", err);

    exit(shared.counter == expected ? EXIT_SUCCESS : EXIT_FAILURE);
}
