#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>


#define MAX_STORE_CUSTOMERS 50 // let's imagine it's a Daiso
#define MAX_STAIR_USERS 4
#define NUM_STAIRS 13
#define FLOOR_1_SEM
#define FLOOR_2_SEM


// When STAIR is 0, there is no direction, 1:ascending, 2: desc
int STAIR_DIRECTION = 0;
// array to contain customers
pthread_t customers[MAX_STORE_CUSTOMERS];

void *descend_stairs(void *customer);
void *ascend_stairs(void *customer);
void semaphore_wait(sem_t *semaphore);
void semaphore_signal(sem_t *semaphore);

// Define global variables to track the users. 
int current_stair_users = 0;
int num_users_crossed = 0;
int num_waiting_to_ascend = 0;
int num_waiting_to_descend = 0;

struct thread_data 
{
    int thread_id;
    int direction;
};

//increment the number of people crossing the stairs
// fix the maximum number of people you can go down
/*Clear some people waiting on floor one
But need a maximum
Need to check the xingCnt to see how many people do it. 
Once that number is exceeded. 
Add mutex and 
Need to change the direction in the semaphore of the stairs
Change the direction in the semaphore
Is it my direction or the other?

Imagine there are two flag people -- one at either direction
All of these variables can be acquired

Have to check direction, number of people waiting to go down
or go up. 

lots of if/else and lots of checking to decide whether it can go
on the stairs. It's time to check

Should keep track of waiting customers in a queue. 
In this code we're not. 

The semaphore is just a person who calls it to wait (it's the first person
trying to go on the stairs. 
if 10 people trying to go up, can acquire the semaphore)

Can use an array to keep pthread objects, write a loop. 
Have to define a maximum

# define NAME 20 // define a constant. :)

Don't want to hold everything in the semaphore.
Once you update all the variables you release the lock and enter the stairs. 

Need to implement crossing.

Once you enter the first step is the release. 

Just need to adjust all the numbers. 

When you create the thread, you need to have the 4 params. 

*/ 


int main(void)
{

    pthread_mutex_t staircase = PTHREAD_MUTEX_INITIALIZER; // initialize the lock.
    // array to keep PThread IDs of created Threads
    // int num_customers = 2;
    // pthread_t thread_id[num_customers]
    pthread_t upstairs_customer;
    // pthread_t downstairs_customer;
    pthread_create(&upstairs_customer, NULL, descend_stairs, NULL);
    // pthread_create(&downstairs_customer, NULL, ascend_stairs, NULL);
    pthread_join(upstairs_customer, NULL);
    // pthread_join(&downstairs_customer, NULL);
    // for (i = 0; i < num_customers; i++) {
        /* HERE IS WHERE THE LOGIC SHOULD GO
        Need locks/semaphore to prevent too many customers?
         */
        // from assignment 12 we create a thread like:
    //     pthread_create(&(tid[i], NULL, down_to_up, NULL));
    // }
}

void *descend_stairs(void *customer_number)
{
    int current_stair;
    for (current_stair = NUM_STAIRS; current_stair >= 0; current_stair--)
    {
        printf("DESC customer: %p is at the top of the stairs.\n", (int *)customer_number);
        if (current_stair == 0)
        {
            printf("DESC customer: %p has reached the first floor.\n\n", (int *)customer_number);
        }
        else
        {
            printf("DESC customer: %p | stair: %d\n", (int *)customer_number, current_stair);
        }
    }
    pthread_exit(NULL);
}


void *ascend_customer(void *customer_number)
{
    int current_stair;
    for (current_stair = 1; current_stair <= NUM_STAIRS; current_stair++)
    {
        printf("ASC customer: %p is at the bottom of the stairs.\n", (int *)customer_number);
        if (current_stair == NUM_STAIRS)
        {
            printf("ASC customer: %p has reached the second floor.\n\n",(int *)customer_number);
        }
        else
        {
            printf("ASC customer: %p | stair: %d\n",(int *)customer_number, current_stair);
        }
    }
    pthread_exit(NULL);
}

// void *customer_scheduler(queue_t floor_1, queue_t floor_2) {
//     // schedules customers here. We may need a queue of customers from each floor.
//     return NULL;
// }

void semaphore_wait(sem_t *semaphore)
{
    // maybe the threads would go here? I dunno.
    if (sem_wait(semaphore) < 0)
    {
        perror("Wait failure.\n");
        exit(EXIT_FAILURE);
    }
}

void semaphore_signal(sem_t *semaphore)
{
    // maybe the threads would go here? I dunno.
    if (sem_post(semaphore) < 0)
    {
        perror("Signal failure.\n");
        exit(EXIT_FAILURE);
    }
}