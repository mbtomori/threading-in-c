#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

// if we want, we can represent the stairs as an array:
int NUM_STAIRS = 13;
// int STAIRS[NUM_STAIRS];
// could put an id of the process in the stairs? or does this over-complicate things?

void *descending_customer(void *arg);
void *ascending_customer(void *arg);
void *use_stairs(void *customer);

int main(void)
{
    // array to keep PThread IDs of created Threads
    // int num_customers = 2;
    // pthread_t thread_id[num_customers]
    
    pthread_t upstairs_customer;
    // pthread_t downstairs_customer;
    pthread_create(&upstairs_customer, NULL, descending_customer, NULL);
    ascending_customer(NULL);
    // pthread_create(&downstairs_customer, NULL, ascending_customer, NULL);
    pthread_join(&upstairs_customer, NULL);
    // pthread_join(&downstairs_customer, NULL);
    // for (i = 0; i < num_customers; i++) {
        /* HERE IS WHERE THE LOGIC SHOULD GO
        Need locks/semaphore to prevent too many customers?
         */
        // from assignment 12 we create a thread like:
    //     pthread_create(&(tid[i], NULL, down_to_up, NULL));
    // }
}

void *descending_customer(void *arg)
{
    int current_stair;
    for (current_stair = NUM_STAIRS; current_stair > 0; current_stair--)
    {
        printf("descending customer is on stair: %d\n", current_stair);
    }
    return NULL;
}


void *ascending_customer(void *arg)
{
    int current_stair;
    for (current_stair = 1; current_stair <= NUM_STAIRS; current_stair++)
    {
        printf("ascending customer is on stair: %d\n",current_stair);
    }
    return NULL;
}

// void *customer_scheduler(queue_t floor_1, queue_t floor_2) {
//     // schedules customers here. We may need a queue of customers from each floor.
//     return NULL;
// }

void *use_stairs(void *customer)
{
    // maybe the threads would go here? I dunno.
    return NULL;
}