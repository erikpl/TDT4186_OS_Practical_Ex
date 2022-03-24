#include "sem.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Declare the semaphore struct */
typedef struct SEM {
    /* Value of the semaphore.
     * When the value is 1, execution is allowed.
     * When the value is 0, it means that another thread is already executing, and the calling thread must wait.
     * When the value is <0, the number shows how many threads are waiting to execute.
     */
    volatile int counter;
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
 * handle for the created semaphore, or NULL if an error occurred.
 */
SEM *sem_init(int initVal) {
    /* Dynamically allocate a semaphore */
    SEM *semaphore = malloc(sizeof(struct SEM));
    /* Check if the dynamic allocation succeeded */
    if (semaphore == NULL) {
        /* Returns NULL if an error occurred */
        return NULL;
    }
    /* Initialize counter to initVal */
    semaphore -> counter = initVal;
    /* Initialize pending_signals to 0 */
    // semaphore -> pending_signals = 0;
    /* Initialize the mutex to an empty mutex.
    *  pthread_mutex_init returns an integer not equal to zero of an error occurred.
    *  https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_mutex_init.html
    */
    pthread_mutex_t mutex;
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        /* Frees semaphore if error occurred. */
        free(semaphore);
        /* Returns NULL if an error occurred. */
        return NULL;
    }
    semaphore -> mutex = mutex;
    /* Initialize the condition to an empty condition 
    *  pthread_cond_init returns an integer not equal to zero of an error occurred.
    *  https://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_cond_init.html
    */
    pthread_cond_t condition;
    if (pthread_cond_init(&condition, NULL) != 0) {
        /* Frees semaphore if error occurred. */
        free(semaphore);
        /* Returns NULL if an error occurred. */
        return NULL;
    }

    semaphore -> condition = condition;

    return semaphore;
}

/* Destroys a semaphore and frees all associated resources.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to destroy
 *
 * Returns:
 *
 * 0 on success, negative value on error.
 *
 * In case of an error, not all resources may have been freed, but 
 * nevertheless the semaphore handle must not be used anymore.
 */
int sem_del(SEM *sem) {
    int error_counter = 0;
    /* Attempt to destroy the mutex */
    if (pthread_mutex_destroy(&(sem -> mutex)) != 0) {
        error_counter += 1;
        printf("Couldn't destroy mutex");
    }

    /* Attempt to destroy the condition */
    if (pthread_cond_destroy(&(sem -> condition)) != 0) {
        error_counter += 2;
        printf("Couldn't destroy condition");
    }

    /* Regardless of errors, free the semaphore handle */
    free(sem);

    /* If an error occurred, -error_counter will be negative.
    *  See above for what the different return values mean.
    *  If not, it will be 0.
    */
    return -error_counter;
}

/* P (wait) operation.
 * 
 * Attempts to decrement the semaphore value by 1. If the semaphore value 
 * is 0, the operation blocks until a V operation increments the value and 
 * the P operation succeeds.
 *
 * Parameters:
 *
 * sem           handle of the semaphore to decrement
 */
void P(SEM *sem) {
    /* Attempts to lock the semaphore's mutex (lock).
     * If the mutex is already locked, the thread will wait
     * until the thread is unlocked.
    */
    pthread_mutex_lock(&(sem -> mutex));

    while ((sem -> counter) == 0) {
        /* Atomically blocks using the semaphore's condition variable.
         * The calling thread will only resume when another thread signals.
         * Will lead to checking the counter variable again.
         * Ignores the return variable.
        */
        pthread_cond_wait(&(sem -> condition), &(sem -> mutex));
    }
    /* Decrement counter to prevent other threads from executing. */
    (sem -> counter)--;

    /* Unlock the semaphore's to enable another thread to operate on the semaphore. */
    pthread_mutex_unlock(&(sem -> mutex));
}

void V(SEM *sem) {
    /* Attempts to lock the semaphore's mutex (lock).
     * If the mutex is already locked, the thread will wait
     * until the thread is unlocked.
    */
    pthread_mutex_lock(&(sem -> mutex));

    // Only signal if incrementing from 0
    // Not sure if this is necessary, but better safe than sorry
    if (sem -> counter == 0) {
        /* Unblock a waiting thread calling pthread_cond_wait.
        * The return variable is ignored.
        */
        pthread_cond_signal(&(sem -> condition));
    }
    /* Increment counter to enable another thread to execute. */
    (sem -> counter)++;

    /* Unlock the semaphore's to enable another thread to operate on the semaphore. */
    pthread_mutex_unlock(&(sem -> mutex));
}