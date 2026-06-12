/**
 * crea alcuni thread e visualizza il loro identificativo `pthread_t`; il
 * confronto tra thread id viene fatto con `pthread_equal`
 *
 * > gcc -std=c99 Esempi-12-06/23-thread-ids.c examples/lib-misc.c -lpthread -o Esempi-12-06/23-thread-ids
 * > ./Esempi-12-06/23-thread-ids
 */

#include "../examples/lib-misc.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 2

struct thread_data {
    pthread_t tid;
    pthread_t main_tid;
    const char *name;
};

void print_thread_info(const char *name, pthread_t main_tid) {
    pthread_t self = pthread_self();

    printf("[%s] pid=%d tid=%lu uguale_al_main=%s\n", name, getpid(),
           (unsigned long)self, pthread_equal(self, main_tid) ? "si" : "no");
}

void *thread_function(void *arg) {
    //sleep(10);
    struct thread_data *data_ptr = (struct thread_data *)arg;
    printf("[%s] thread creato con pthread_create\n", data_ptr->name);
    print_thread_info(data_ptr->name, data_ptr->main_tid);
    printf("[%s] thread terminato\n", data_ptr->name);
    return NULL;
}

int main(void) {
    int err;
    pthread_t main_tid = pthread_self();
    struct thread_data threads[NUM_THREADS] = {
        {0, 0, "Giovanni"},
        {0, 0, "T2"},
    };

    print_thread_info("main", main_tid);

    for (int i = 0; i < NUM_THREADS; i++) {
        threads[i].main_tid = main_tid;
        if ((err = pthread_create(&threads[i].tid, NULL, thread_function, (void *)&threads[i])) != 0)
            exit_with_err("pthread_create", err);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        if ((err = pthread_join(threads[i].tid, NULL)) != 0)
            exit_with_err("pthread_join", err);
    
    printf("[main] terminato\n");
    exit(EXIT_SUCCESS);
}
