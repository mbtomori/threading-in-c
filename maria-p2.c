#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

// The stairs are represented as an int. When STAIRS are positive, customers are going
// upstairs. If negative, customers are going down the stairs.

int DIRECTION = 0;
// could put an id of the process in the stairs? or does this over-complicate things?

void *descending_customer(void *arg);
void *ascending_customer(void *arg);
void *use_stairs(void *customer);

int main(void)
{
    pthread_mutex_t staircase; // initialize the lock.
    // array to keep PThread IDs of created Threads
    // int num_customers = 2;
    // pthread_t thread_id[num_customers]
    int direction = 0; // if the direction is negative, a person can descend, if positive you can ascend
    int STAIRS[NUM_STAIRS];
    pthread_t upstairs_customer;
    // pthread_t downstairs_customer;
    pthread_create(&upstairs_customer, NULL, descending_customer, 2);
    ascending_customer(2);
    descending_customer(1);
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

void *descending_customer(void *customer_number)
{
    int current_stair;
    for (current_stair = NUM_STAIRS; current_stair >= 0; current_stair--)
    {
        printf("DESC customer: %d is at the top of the stairs.\n", (int)customer_number);
        if (current_stair == 0)
        {
            printf("DESC customer: %d has reached the first floor.\n\n", (int)customer_number);
        }
        else
        {
            printf("DESC customer: %d | stair: %d\n", (int)customer_number, current_stair);
        }
    }
    return NULL;
}


void *ascending_customer(void *customer_number)
{
    int current_stair;
    for (current_stair = 1; current_stair <= NUM_STAIRS; current_stair++)
    {
        printf("ASC customer: %d is at the bottom of the stairs.\n", (int)customer_number);
        if (current_stair == NUM_STAIRS)
        {
            printf("ASC customer: %d has reached the second floor.\n\n",(int)customer_number);
        }
        else
        {
            printf("ASC customer: %d | stair: %d\n",(int)customer_number, current_stair);
        }
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