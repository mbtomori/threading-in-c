
/*
Stairs crossing problem using pThreads and Semaphores
*/
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#ifndef MAX_STORE_CUSTOMERS
// TODO
// #define MAX_STORE_CUSTOMERS : which is the maximum number of customers/threads in the system to test
#define MAX_STORE_CUSTOMERS 10
// #define MAX_STAIR_USERS: how many customer can be on the stairs at the same time
#define MAX_STAIR_USERS 4
// you can also define other constants for your "prevent deadlock" or "prevent starvation" algorithm
#endif 


//TODO 
// create a struct as the cusomer's data. Contains the customer's ID.
typedef struct customer_t
{
    int id;
} customer_t;

typedef struct lightswitch_t
{
    int counter;
} lightswitch_t;


// counting semaphores
sem_t first_floor_gatekeeper;
sem_t second_floor_gatekeeper;

// direction semaphores
sem_t up_direction;
sem_t down_direction;

//initialize the lightswitches
lightswitch_t up_switch;
lightswitch_t down_switch;

// determine if you're the first person on stairs
sem_t empty_stairs;

// binary turnstile semaphore to avoid starvation
sem_t stair_manager;

//Functions to "use the stairs"
void *ascend_stairs(void *arg);
void *descend_stairs(void *arg);

//Semaphore wait and signal functions
void semaphore_wait(sem_t *sem);
void semaphore_signal(sem_t *sem);

// Helper function to reset the direction
void reset_direction(char *direction);

//lightswitch functions
void switch_lock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore);
void switch_unlock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore);


int main(void)
{
    printf("Project 2: Customer crossing problem using pThreads and Semaphores\n");
    printf("*****************\n\n");
    fflush(stdout);

    customer_t customer_data[MAX_STORE_CUSTOMERS];
    pthread_t store_customers[MAX_STORE_CUSTOMERS];

    // set values of switches
    up_switch.counter = 0;
    down_switch.counter = 0;

    int errCheck;

    // initialize the semaphores and check they were initialized properly
    if (sem_init(&first_floor_gatekeeper, 0, (unsigned int)MAX_STAIR_USERS) < 0
        || sem_init(&second_floor_gatekeeper, 0, (unsigned int)MAX_STAIR_USERS) < 0
        || sem_init(&stair_manager, 0, (unsigned int)1) < 0
        || sem_init(&empty_stairs, 0, (unsigned int)1) < 0
        || sem_init(&up_direction, 0, (unsigned int)1) < 0
        || sem_init(&down_direction, 0, (unsigned int)1) < 0)
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
    sem_destroy(&stair_manager);
    sem_destroy(&up_direction);
    sem_destroy(&down_direction);
    sem_destroy(&empty_stairs);

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

    // Check stair_manager lock to see if there are others on the stairs.
    semaphore_wait(&stair_manager);
    printf("****\nCustomer %d checking stair_manager lock.\n", customer_id);
    switch_lock(&down_switch, &down_direction, &empty_stairs);
    printf("****\nCustomer %d first one.\n", customer_id);
    semaphore_signal(&stair_manager);
    printf("****\nCustomer %d release manager.\n", customer_id);
    semaphore_wait(&second_floor_gatekeeper);

    printf("**Customer %d is descending the stairs.\n", customer_id);
    sleep(1);
    printf("Customer %d reached the first floor.\n", customer_id);
    semaphore_signal(&second_floor_gatekeeper);
    switch_unlock(&down_switch, &down_direction, &empty_stairs);
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

    // Check stair_manager lock to see if there are others on the stairs.
    semaphore_wait(&stair_manager);
    switch_lock(&up_switch, &up_direction, &empty_stairs);
    semaphore_signal(&stair_manager);
    semaphore_wait(&first_floor_gatekeeper);

    printf("**Customer %d is ascending the stairs.\n", customer_id);
    sleep(1);
    printf("Customer %d reached the second floor.\n", customer_id);
    semaphore_signal(&first_floor_gatekeeper);
    switch_unlock(&up_switch, &up_direction, &empty_stairs);
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


// lightswitch implementation to set the direction of the staircase.
void switch_lock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore)
{
    semaphore_wait(direction_semaphore);
    direction->counter++;
    if (direction->counter == 1)
    {
        semaphore_wait(locking_semaphore);
    }
    semaphore_signal(direction_semaphore);
}

// lightswitch implementation to unset the staircase direction.
void switch_unlock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore)
{
    semaphore_wait(direction_semaphore);
    direction->counter--;
    if (direction->counter == 0)
    {
        semaphore_signal(locking_semaphore);
    }
    semaphore_signal(direction_semaphore);
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