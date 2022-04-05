
/*
Stairs crossing problem using pThreads and Semaphores
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef MAX_STORE_CUSTOMERS
// TODO
// #define MAX_STORE_CUSTOMERS : which is the maximum number of customers/threads in the system to test
#define MAX_STORE_CUSTOMERS 15
// #define MAX_STAIR_USERS: how many customer can be on the stairs at the same time
#define MAX_STAIR_USERS 4
// you can also define other constants for your "prevent deadlock" or "prevent starvation" algorithm
#endif 


//TODO 
// create a struct as Thread's data. Contains the thread's ID.
typedef struct customer_t
{
    int id;
} customer_t;

/*
 *  the following variables are created to:
 *  1. prevent deadlock
 *  2. prevent starvation
 *  3. allow more than one customer to use the stairs in the same direction in an “efficient” way
       that you determine
 */

int current_stair_users = 0;
int num_users_crossed = 0;
int number_waiting_to_ascend = 0;
int num_waiting_to_descend = 0;


/* 
 * We used an enum instead of int for readability.
 */
enum {none, up, down} stair_direction;
sem_t first_floor_gatekeeper;
sem_t second_floor_gatekeeper;


//Functions to "use the stairs"
void *ascend_stairs(void *arg);
void *descend_stairs(void *arg);

//Semaphore wait and signal functions
void semaphore_wait(sem_t *sem);
void semaphore_signal(sem_t *sem);


int main(void)
{
    printf("Project 2: Customer crossing problem using pThreads and Semaphores\n");
    printf("*****************\n\n");
    fflush(stdout);

    // TODO
    /*
     *  initialize your pthread, semaphore and other variables you will need
     *  please do error check if the program fail to pthread_create()
     */

    customer_t customer_data[MAX_STORE_CUSTOMERS];
    pthread_t store_customers[MAX_STORE_CUSTOMERS ];
    stair_direction = none;

    int errCheck;

    // initialize the semaphores and check they were initialized properly
    if (sem_init(&first_floor_gatekeeper, 0, (unsigned int)1) < 0
        || sem_init(&second_floor_gatekeeper, 0, (unsigned int)1) < 0)
    {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    printf("Parent Process PID: %d \n", getpid()); //parent PID
    fflush(stdout);
    

    // TODO
     for (int i = 0; i < MAX_STORE_CUSTOMERS ; ++i)
     {

        void *thread_func;

        customer_data[i].id = i;

        if (rand() % MAX_STORE_CUSTOMERS  <= MAX_STORE_CUSTOMERS / 2)
        {
            thread_func = ascend_stairs;
        } 
        else
        {
            thread_func = descend_stairs;
        }

        if ((errCheck = pthread_create(&store_customers[i], NULL, thread_func, &customer_data[i])))
        {
                fprintf(stderr, "error: pthread_create, %d\n", errCheck);
                return EXIT_FAILURE;
            }
        }
    
    for (int i = 0; i < MAX_STORE_CUSTOMERS ; ++i)
    {
        if ((errCheck = pthread_join(store_customers[i], NULL)))
        {
            fprintf(stderr, "error: pthread_join, %d\n", errCheck);
        }
    }

    // destroy the semaphores to prevent memory leaks
    sem_destroy(&second_floor_gatekeeper);
    sem_destroy(&first_floor_gatekeeper);

    return EXIT_SUCCESS;
}

/*
* Function for the customer to descend to lower level
*/
void *descend_stairs(void *customer_thread)
{
    customer_t *data = (customer_t *)customer_thread;

    int customer_id = data->id;

    printf("****\nCustomer %d would like to descend to the first floor.\n", customer_id);
    // acquire the lock to update the count
    semaphore_wait(&second_floor_gatekeeper);
    num_waiting_to_descend++;

    // Can the customer descend?
    if ((stair_direction == none || stair_direction == down)
       && (current_stair_users + num_users_crossed < MAX_STAIR_USERS))
    {
        printf("**Customer %d is permitted to descend the stairs.\n", customer_id);
        if (stair_direction == none)
        {
            stair_direction = down;
        }
        current_stair_users++;
        num_waiting_to_descend--;
    }
    semaphore_signal(&second_floor_gatekeeper);
    
    // customer descends the stairs.
    printf("**Customer %d is descending the stairs.\n", customer_id);
    sleep(1);

    // customer reached the first floor and checks in with gatekeeper
    semaphore_wait(&first_floor_gatekeeper);
    printf("Customer %d reached the first floor.\n", customer_id);
    num_users_crossed++;
    current_stair_users--;

    // do we need to reset the direction of the stairs?
    if (current_stair_users + num_users_crossed >= MAX_STAIR_USERS)
    {
        printf("%d Customers have descended the stairs. Resetting the direction.\n", num_users_crossed);
        stair_direction = none;
        num_users_crossed = 0;
        current_stair_users = 0;
    }
    semaphore_signal(&first_floor_gatekeeper);
    pthread_exit(NULL);
}


/*
* Function for the customer to 2nd level
*/
void *ascend_stairs(void *customer_thread)
{
    customer_t *data = (customer_t *)customer_thread;

    int customer_id = data->id;

    printf("****\nCustomer %d would like to ascend to the second floor.\n", customer_id);
    // acquire the lock to update the count
    semaphore_wait(&first_floor_gatekeeper);
    num_waiting_to_ascend++;

    // Can the customer ascend?
    if ((stair_direction == none || stair_direction == up)
       && (current_stair_users + num_users_crossed < MAX_STAIR_USERS))
    {
        printf("**Customer %d is permitted to ascend the stairs.\n", customer_id);
        if (stair_direction == none)
        {
            stair_direction = up;
        }
        current_stair_users++;
        num_waiting_to_ascend--;
    }
    semaphore_signal(&first_floor_gatekeeper);
    
    // customer ascends the stairs.
    printf("**Customer %d is ascending the stairs.\n", customer_id);
    sleep(1);

    // customer reached the first floor and checks in with gatekeeper
    semaphore_wait(&second_floor_gatekeeper);
    printf("Customer %d reached the second floor.\n", customer_id);
    num_users_crossed++;
    current_stair_users--;

    // do we need to reset the direction of the stairs?
    if (current_stair_users + num_users_crossed >= MAX_STAIR_USERS)
    {
        printf("%d Customers have ascended the stairs. Resetting the direction.\n", num_users_crossed);
        stair_direction = none;
        num_users_crossed = 0;
        current_stair_users = 0;
    }
    semaphore_signal(&second_floor_gatekeeper);
    pthread_exit(NULL);
}

// helper functions
/*
 *  Error-checked semaphore wait.
 */
void semaphore_wait(sem_t *sem)
{
    if (sem_wait(sem) < 0)
    {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
}

/*
 *  Error-checked semaphore signal.
 */
void semaphore_signal(sem_t *sem)
{
    if (sem_post(sem) < 0)
    {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
}

/*
 *  Expected test result:
 * Please print logs to show the customers' status for testing. Here is an example:
Project 2: Customer crossing problem using pThreads and Semaphores
*****************
Parent Process PID: 86672 
****
Thread 0: About to cross
customer is Crossing from B to A
Crossing Finished
Signaling stair_lock
Using Stairs 
Finished Stairs . Waiting for stair_lock 
stair_lock Passed
Crossing Direction Reset 
Signaling stair_lock
****
Thread 1: About to cross
customer is Crossing from B to A
Crossing Finished
Signaling stair_lock
Using Stairs 
Finished Stairs . Waiting for stair_lock 
stair_lock Passed
Crossing Direction Reset 
Signaling stair_lock
****
Thread 2: customer Crossing from A to B
customer is Crossing from A to B
Signaling stair_lock
There are 0 customers waiting 
Using Stairs 
The customer will finish crossing in 1 seconds 
Finished Stairs. Waiting for stair_lock 
stair_lock Passed
Crossing Direction Reset 
****


}
 */