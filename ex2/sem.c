#include "sem.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Declare the semaphore struct */
typedef struct SEM {
    volatile int counter;
    // TODO: not sure if this one is needed
    volatile int pending_signals;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} SEM;

/* Creates a new semaphore.
 *
 * This function creates a new semaphore. If an error occurs during the 
 * initialization, the implementation shall free all resources already 
 * allocated so far.
 *
 * Parameters:
 *
 * initVal      the initial value of the semaphore
 *
 * Returns:
 *
 * handle for the created semaphore, or NULL if an error occured.
 */
SEM *sem_init(int initVal) {
    /* Dynamically allocate a semaphore */
    SEM *semaphore = malloc(sizeof(struct SEM));
    /* Check if the dynamic allocation succeeded */
    if (semaphore == NULL) {
        /* Returns NULL if an error occured */
        return NULL;
    }
    /* Initialize counter to initVal */
    semaphore -> counter = initVal;
    /* Initialize pending_signals to 0 */
    semaphore -> pending_signals = 0;
    /* Initialize the mutex to an empty mutex.
    *  pthread_mutex_init returns an integer not equal to zero of an error occured. 
    *  https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_mutex_init.html
    */
    if (pthread_mutex_init(&(semaphore -> mutex), NULL) != 0) {
        /* Returns NULL if an error occured */
        return NULL;
    }

    /* Initialize the condition to an empty condition 
    *  pthread_cond_init returns an integer not equal to zero of an error occured. 
    *  https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_cond_init.html
    */
    if (pthread_cond_init(&(semaphore -> condition), NULL) != 0) {
        /* Returns NULL if an error occured */
        return NULL;
    }

    return semaphore;
}

int main() {
    int init_val1 = 0;
    int init_val2 = 1;
    
    SEM *semaphore1 = sem_init(init_val1);
    SEM *semaphore2 = sem_init(init_val2);

    printf("\nAddress of semaphore1: %p\n", (void*) semaphore1);
    printf("\nAddress of semaphore2: %p\n", (void*) semaphore2);

}