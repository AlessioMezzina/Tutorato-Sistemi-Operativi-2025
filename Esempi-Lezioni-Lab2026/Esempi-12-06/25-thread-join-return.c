/**
 * usa `pthread_join` per attendere thread specifici e recuperarne il valore di
 * ritorno; un thread termina con `return`, l'altro con `pthread_exit`
 *
 * > gcc -std=c99 Esempi-12-06/25-thread-join-return.c examples/lib-misc.c -lpthread -o Esempi-12-06/25-thread-join-return
 * > ./Esempi-12-06/25-thread-join-return
 */

#include "../examples/lib-misc.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 2

struct thread_data {
    pthread_t tid;
    const char *name;
    int value;
    int use_pthread_exit;
};

struct thread_result {
    const char *name;
    int value;
    int cube;
};

void *thread_function(void *arg) {
    struct thread_data *data_ptr = (struct thread_data *)arg;
    struct thread_result *result = malloc(sizeof(*result));

    if (result == NULL)
        exit_with_sys_err("malloc");

    result->name = data_ptr->name;
    result->value = data_ptr->value;
    result->cube = data_ptr->value * data_ptr->value * data_ptr->value;

    printf("[%s] calcolato cubo di %d\n", data_ptr->name, data_ptr->value);

    if (data_ptr->use_pthread_exit)
        pthread_exit((void *)result);

    return (void *)result;
}

int main(void) {
    int err;
    struct thread_data threads[NUM_THREADS] = {
        {0, "T1", 2, 0},
        {0, "T2", 4, 1},
    };

    for (int i = 0; i < NUM_THREADS; i++)
        if ((err = pthread_create(&threads[i].tid, NULL, thread_function,
                                  (void *)&threads[i])) != 0)
            exit_with_err("pthread_create", err);

    for (int i = 0; i < NUM_THREADS; i++) {
        struct thread_result *result;

        if ((err = pthread_join(threads[i].tid, (void **)&result)) != 0)
            exit_with_err("pthread_join", err);

        printf("[main] %s ha restituito: %d^3=%d\n", result->name,
               result->value, result->cube);
        free(result);
    }

    exit(EXIT_SUCCESS);
}
