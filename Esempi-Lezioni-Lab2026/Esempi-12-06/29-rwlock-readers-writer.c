/**
 * mostra un lock lettori/scrittori: piu' lettori possono entrare insieme, lo
 * scrittore acquisisce invece il lock in modo esclusivo
 *
 * > gcc -std=c99 Esempi-12-06/29-rwlock-readers-writer.c examples/lib-misc.c -lpthread -o Esempi-12-06/29-rwlock-readers-writer
 * > ./Esempi-12-06/29-rwlock-readers-writer
 */

#include "../examples/lib-misc.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_READERS 3

struct shared_data {
    int value;
    pthread_rwlock_t lock;
};

struct reader_data {
    pthread_t tid;
    unsigned int id;
    struct shared_data *shared;
};

struct writer_data {
    pthread_t tid;
    struct shared_data *shared;
};

void *reader_function(void *arg) {
    int err;
    struct reader_data *data_ptr = (struct reader_data *)arg;

    if ((err = pthread_rwlock_rdlock(&data_ptr->shared->lock)) != 0)
        exit_with_err("pthread_rwlock_rdlock", err);

    printf("[R%u] lettura valore=%d\n", data_ptr->id,
           data_ptr->shared->value);
    sleep(1);
    printf("[R%u] fine lettura\n", data_ptr->id);

    if ((err = pthread_rwlock_unlock(&data_ptr->shared->lock)) != 0)
        exit_with_err("pthread_rwlock_unlock", err);

    return NULL;
}

void *writer_function(void *arg) {
    int err;
    struct writer_data *data_ptr = (struct writer_data *)arg;

    sleep(1);
    if ((err = pthread_rwlock_wrlock(&data_ptr->shared->lock)) != 0)
        exit_with_err("pthread_rwlock_wrlock", err);

    data_ptr->shared->value += 10;
    printf("[W] scrittura nuovo valore=%d\n", data_ptr->shared->value);

    if ((err = pthread_rwlock_unlock(&data_ptr->shared->lock)) != 0)
        exit_with_err("pthread_rwlock_unlock", err);

    return NULL;
}

int main(void) {
    int err;
    struct shared_data shared = {100, PTHREAD_RWLOCK_INITIALIZER};
    struct reader_data readers[NUM_READERS];
    struct writer_data writer = {0, &shared};

    for (unsigned int i = 0; i < NUM_READERS; i++) {
        readers[i].id = i + 1;
        readers[i].shared = &shared;
        if ((err = pthread_create(&readers[i].tid, NULL, reader_function,
                                  (void *)&readers[i])) != 0)
            exit_with_err("pthread_create", err);
    }

    if ((err = pthread_create(&writer.tid, NULL, writer_function,
                              (void *)&writer)) != 0)
        exit_with_err("pthread_create", err);

    for (int i = 0; i < NUM_READERS; i++)
        if ((err = pthread_join(readers[i].tid, NULL)) != 0)
            exit_with_err("pthread_join", err);
    if ((err = pthread_join(writer.tid, NULL)) != 0)
        exit_with_err("pthread_join", err);

    if ((err = pthread_rwlock_destroy(&shared.lock)) != 0)
        exit_with_err("pthread_rwlock_destroy", err);

    exit(EXIT_SUCCESS);
}
