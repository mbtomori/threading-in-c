#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

// if we want, we can represent the stairs as an array:
int NUM_STAIRS = 13;
int STAIRS[NUM_STAIRS]; // could put an id of the process in the stairs? or does this over-complicate things?

/* This function represents a customer who starts upstairs and wants to 
go downstairs. */
void *upstairs_customer (void *vargp)
{
    for (int i = NUM_STAIRS; i > 0; i--)
    {
        printf("process: %d is on stair: %d", process_id, i+1);
    }
    return NULL;
}

/* This function represents a customer who starts downstairs and wants to 
go upstairs. */
void *downstairs_customer (void *vargp)
{
    for (int i = 0; i < NUM_STAIRS; i++)
    {
        printf("process: %d is on stair: %d", process_id, i+1);
    }
    return NULL;
}

void *customer_scheduler(queue_t floor_1, queue_t floor_2) {
    // schedules customers here. We may need a queue of customers from each floor.
    return NULL;
}

void *use_stairs(void *customer)
{
    // maybe the threads would go here? I dunno.
}

int main(void)
{
    // array to keep PThread IDs of created Threads
    int num_customers = 2;
    pthread_t thread_id[num_customers]

    for (i = 0; i < num_customers; i++) {
        /* HERE IS WHERE THE LOGIC SHOULD GO
        Need locks/semaphore to prevent too many customers?
         */
        // from assignment 12 we create a thread like:
        pthread_create(&(tid[i], NULL, down_to_up, NULL));
    }
}