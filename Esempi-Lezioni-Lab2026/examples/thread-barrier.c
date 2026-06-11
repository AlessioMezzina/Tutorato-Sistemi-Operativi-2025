/**
 * esempio che mostra come 1+n (il thread principale più altri n addizionali) si
 * possono coordinare su alcuni passaggi (partenza e terminazione) utilizzando
 * una barriera; gli 1+n thread sono trattati alla pari e alla fine viene usato
 * il thread prescelto dalla barriera come gestore dell'ultima fase
 */

#include "lib-misc.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_ADDITIONAL_THREADS 5
#define NUM_TOTAL_THREADS (NUM_ADDITIONAL_THREADS + 1)
#define MAX_RANDOM_PAUSE 5

typedef struct {
    pthread_t tid;
    unsigned int id;

    pthread_barrier_t *barrier_ptr;
} thread_data_t;

void *thread_function(void *arg) {
    assert(arg);
    int err;
    thread_data_t *data_ptr = (thread_data_t *)arg;

    if ((err = pthread_barrier_wait(data_ptr->barrier_ptr)) > 0)
        exit_with_err("pthread_barrier_wait", err);

    printf("[T%u] thread partito!\n", data_ptr->id);

    sleep(rand() % MAX_RANDOM_PAUSE); // range: [1 ... MAX]

    printf("[T%u] thread terminato!\n", data_ptr->id);

    if ((err = pthread_barrier_wait(data_ptr->barrier_ptr)) > 0)
        exit_with_err("pthread_barrier_wait", err);

    if (err == PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("[T%u] thread eletto come coordinatore sull'ultima "
               "sincronizzazione sulla barriera\n",
               data_ptr->id);
        printf("[T%u] tutti i thread (tranne il sottoscritto) hanno terminato "
               "il loro compito\n",
               data_ptr->id);

        // non indispensabile ma è una buona abitudine...
        if ((err = pthread_barrier_destroy(data_ptr->barrier_ptr)))
            exit_with_err("pthread_barrier_destroy", err);
        // NB: questa operazione sarà fatta una sola volta dal thread
        // coordinatore
    }

    return (NULL);
}

int main(int argc, char *argv[]) {
    int err;
    thread_data_t thread_data[NUM_TOTAL_THREADS];

    srand(time(NULL));

    printf(
        "creazione di %d+1 thread con sincronizzazione tramite barriera...\n",
        NUM_ADDITIONAL_THREADS);

    // creazione della barriera condivisa
    pthread_barrier_t barrier;
    if ((err = pthread_barrier_init(&barrier, NULL, NUM_TOTAL_THREADS)))
        exit_with_err("pthread_barrier_init", err);

    // il thread principale verrà gestito alla pari come tutti gli altri
    thread_data[0].id = 0;
    thread_data[0].barrier_ptr = &barrier;

    // creazione dei thread addizionali (oltre quello principale del main)
    for (int i = 1; i <= NUM_ADDITIONAL_THREADS; i++) {
        thread_data[i].id = i;
        thread_data[i].barrier_ptr = &barrier;
        if ((err = pthread_create(&thread_data[i].tid, NULL, thread_function,
                                  (void *)(&thread_data[i]))))
            exit_with_err("pthread_create", err);
    }

    sleep(2); // gli altri comunque devono aspettare il thread principale (T0)

    printf("pronti alla partenza...\n");

    // il thread principale (T0) eseguirà un'altra istanza della funzione
    // `thread_function` alla pari con tutti gli altri
    pthread_exit(thread_function((void *)(&thread_data[0])));

    // il flusso di esecuzione non arriverà mai qui

    // NB: il processo terminerà solo quando l'ultimo thread avrà terminato (a
    // prescindere da chi sarà)
}