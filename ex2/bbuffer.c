#include "bbuffer.h"
#include "sem.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

typedef struct BNDBUF {
    // Buffer capacity
    unsigned int size;
    // "read head", keeps track of where the "next" element is located
    unsigned int read_index;
    // "write head", keeps track of where to add the next element
    unsigned int write_index;
    // The buffer itself
    int *buffer;
    // Number of (active) elements in the buffer
    unsigned int num_of_elements;
    // Semaphore used to indicate if the buffer is empty
    SEM *empty;
    // Semaphore used to indicate if the buffer is full
    SEM *full;
} BNDBUF;

int bb_get(BNDBUF *bb) {
    // Wait for the buffer to become non-empty, i.e. have at least one full spot
    P(bb -> full);

    unsigned int current_read_index = bb -> read_index;
    // Read from the "top of the array"
    int read_value = bb -> buffer[current_read_index];
    // Update the read index. Use modulo to perform "wrap-around"
    bb -> read_index = (current_read_index + 1) % (bb -> size);
    // Decrement the number of elements in the buffer
    bb -> num_of_elements--;

    // Increment empty, signaling that a spot has been emptied
    V(bb -> empty);

    return read_value;
}

void bb_add(BNDBUF *bb, int fd) {
    // Wait for the buffer to become non-full, i.e. have empty spots
    P(bb -> empty);

    unsigned int current_write_index = bb -> write_index;
    // Write fd to the array
    bb -> buffer[current_write_index] = fd;
    // Update the write index. Use modulo to perform "wrap-around"
    bb -> write_index = (current_write_index + 1) % (bb -> size);
    // Increment number of elements in the buffer
    bb -> num_of_elements++;

    // Increment full, signaling that a spot is now empty
    V(bb -> full);
}

/*
 * Returns a pointer to the created and initialized BNDBUF struct.
 * Remember to check that the returned value is not NULL
*/
BNDBUF* bb_init(unsigned int size) {
    // The struct will consist of 4 unsigned integers, and size regular integers
    // Unsigned ints and ints should be the same thing, but doing it like this as a precaution
    BNDBUF *bb = malloc(sizeof(struct BNDBUF));
    bb -> num_of_elements = 0;
    bb -> size = size;
    bb -> read_index = 0;
    bb -> write_index = 0;
    // Dynamically allocate memory to store the int array
    bb -> buffer = malloc(size * sizeof(int));
    // Initialize empty to 0 since the buffer is empty when initialized
    bb -> empty = sem_init(size);
    // Initialize full to 1 since the buffer is not full when initialized
    bb -> full = sem_init(0);

    return bb;
}

void bb_del(BNDBUF *bb) {
    // Free the empty semaphore
    sem_del(bb -> empty);
    // Free the full semaphore
    sem_del(bb -> full);
    // Free the dynamically allocated buffer array
    free(bb -> buffer);
}