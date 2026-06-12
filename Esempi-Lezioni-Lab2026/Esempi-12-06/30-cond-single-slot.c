/**
 * implementa una coda di un solo elemento con mutex e variabile condizione:
 * il consumatore aspetta in un ciclo `while` finche' c'e' un dato disponibile
 *
 * > gcc -std=c99 Esempi-12-06/30-cond-single-slot.c examples/lib-misc.c -lpthread -o Esempi-12-06/30-cond-single-slot
 * > ./Esempi-12-06/30-cond-single-slot
 */

#include "../examples/lib-misc.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_ITEMS 5

struct shared_data { // "zona" di memoria condivisa tra tutti i thread
    int value;
    int available;
    int closed;
    pthread_mutex_t lock;
    pthread_cond_t changed;
};

struct thread_data { 
    pthread_t tid; // variabile "privata" di un thread

    struct shared_data *shared; // riferimento alla zona di memoria condivisa
};

void *producer_function(void *arg) {
    int err;
    struct thread_data *data_ptr = (struct thread_data *)arg;

    for (int i = 1; i <= NUM_ITEMS; i++) {
        if ((err = pthread_mutex_lock(&data_ptr->shared->lock)) != 0)
            exit_with_err("pthread_mutex_lock", err);

        while (data_ptr->shared->available)
            if ((err = pthread_cond_wait(&data_ptr->shared->changed,
                                         &data_ptr->shared->lock)) != 0)
                exit_with_err("pthread_cond_wait", err);


        data_ptr->shared->value = i * 10;
        data_ptr->shared->available = 1;
        printf("[P] prodotto valore=%d\n", data_ptr->shared->value);

        if ((err = pthread_cond_signal(&data_ptr->shared->changed)) != 0)
            exit_with_err("pthread_cond_signal", err);
        if ((err = pthread_mutex_unlock(&data_ptr->shared->lock)) != 0)
            exit_with_err("pthread_mutex_unlock", err);
        sleep(1);
    }

    if ((err = pthread_mutex_lock(&data_ptr->shared->lock)) != 0)
        exit_with_err("pthread_mutex_lock", err);
    data_ptr->shared->closed = 1;
    if ((err = pthread_cond_broadcast(&data_ptr->shared->changed)) != 0)
        exit_with_err("pthread_cond_broadcast", err);
    if ((err = pthread_mutex_unlock(&data_ptr->shared->lock)) != 0)
        exit_with_err("pthread_mutex_unlock", err);

    return NULL;
}

void *consumer_function(void *arg) {
    int err;
    struct thread_data *data_ptr = (struct thread_data *)arg;

    while (1) {
        if ((err = pthread_mutex_lock(&data_ptr->shared->lock)) != 0)
            exit_with_err("pthread_mutex_lock", err);

        while (!data_ptr->shared->available && !data_ptr->shared->closed)
            if ((err = pthread_cond_wait(&data_ptr->shared->changed,
                                         &data_ptr->shared->lock)) != 0)
                exit_with_err("pthread_cond_wait", err);

        if (!data_ptr->shared->available && data_ptr->shared->closed) {
            if ((err = pthread_mutex_unlock(&data_ptr->shared->lock)) != 0)
                exit_with_err("pthread_mutex_unlock", err);
            break;
        }

        printf("[C] consumato valore=%d\n", data_ptr->shared->value);
        data_ptr->shared->available = 0;

        if ((err = pthread_cond_signal(&data_ptr->shared->changed)) != 0)
            exit_with_err("pthread_cond_signal", err);
        if ((err = pthread_mutex_unlock(&data_ptr->shared->lock)) != 0)
            exit_with_err("pthread_mutex_unlock", err);
    }

    printf("[C] nessun altro dato: termino\n");

    return NULL;
}

int main(void) {
    int err;
    struct shared_data shared = {0, 0, 0, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER};
    struct thread_data producer = {0, &shared};
    struct thread_data consumer = {0, &shared};

    if ((err = pthread_create(&consumer.tid, NULL, consumer_function,
                              (void *)&consumer)) != 0)
        exit_with_err("pthread_create", err);
    if ((err = pthread_create(&producer.tid, NULL, producer_function,
                              (void *)&producer)) != 0)
        exit_with_err("pthread_create", err);

    if ((err = pthread_join(producer.tid, NULL)) != 0)
        exit_with_err("pthread_join", err);
    if ((err = pthread_join(consumer.tid, NULL)) != 0)
        exit_with_err("pthread_join", err);

    if ((err = pthread_cond_destroy(&shared.changed)) != 0)
        exit_with_err("pthread_cond_destroy", err);
    if ((err = pthread_mutex_destroy(&shared.lock)) != 0)
        exit_with_err("pthread_mutex_destroy", err);

    exit(EXIT_SUCCESS);
}
