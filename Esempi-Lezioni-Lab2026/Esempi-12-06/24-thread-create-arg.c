/**
 * mostra il passaggio di un argomento a `pthread_create`: ogni thread riceve
 * una struttura con dati privati e calcola il quadrato del numero assegnato
 *
 * > gcc -std=c99 Esempi-12-06/24-thread-create-arg.c examples/lib-misc.c -lpthread -o Esempi-12-06/24-thread-create-arg
 * > ./Esempi-12-06/24-thread-create-arg
 */

#include "../examples/lib-misc.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10

struct thread_data {
    pthread_t tid;
    const char *name;
    int value;
};

void *thread_function(void *arg) { // routine di partenza a seguite di una pthread_create
    struct thread_data *data_ptr = (struct thread_data *)arg;
    int value = data_ptr->value;
    const char *name = data_ptr->name;

    int square = value * value;
    printf("[%s] valore=%d quadrato=%d\n", name, value, square);
    printf("[%s] thread terminato\n", name);
    pthread_exit(NULL);
}

int main(void) {
    int err;
    struct thread_data threads[NUM_THREADS] = {
        {0, "T1", 3},
        {0, "T2", 5},
        {0, "T3", 7},
        {0, "T4", 9},
        {0, "T5", 11},
        {0, "T6", 13},
        {0, "T7", 15},
        {0, "T8", 17},
        {0, "T9", 19},
        {0, "T10", 21}
    };

    
    for (int i = 0; i < NUM_THREADS; i++) {
        printf("[main] creo %s passandogli una struct dedicata\n", threads[i].name);
        if ((err = pthread_create(&threads[i].tid, NULL, thread_function, (void *)&threads[i])) != 0)
            exit_with_err("pthread_create", err);
    }

    //stampa tutti i thread[i].tid
    //for (int i = 0; i < NUM_THREADS; i++)
    //    printf("[main] thread %s ha tid=%lu\n", threads[i].name, (unsigned long)threads[i].tid);

    for (int i = 0; i < NUM_THREADS; i++)
        if ((err = pthread_join(threads[i].tid, NULL)) != 0)
            exit_with_err("pthread_join", err);

    
    
    exit(EXIT_SUCCESS);
}
