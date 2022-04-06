
/*
Stairs crossing problem using pThreads and Semaphores
*/
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// DEFINE CONSTANTS
#define MAX_STORE_CUSTOMERS 10 // max customers in the store
#define MAX_STAIR_USERS 4  // max customers who can use the stairs simultaneously


/*  **STRUCTS **
 This struct defines a customer */
typedef struct customer_t
{
    int id;
} customer_t;

// This struct creates the most basic of lightswitches
// that contains a counter (to determine if someone is
// the first to the stairs)
typedef struct lightswitch_t
{
    int counter;
} lightswitch_t;

// ** SEMAPHORE DEFININTION
// counting semaphores
// These limit the number of people who can use the stairs simultaneously
sem_t first_floor_gatekeeper;
sem_t second_floor_gatekeeper;

// These semaphores protect the direction the stair is currently supporting
sem_t up_direction;
sem_t down_direction;

//These are the semaphores used in the lightswitches
lightswitch_t up_switch;
lightswitch_t down_switch;

// Tracks whether the stairway is currently empty
sem_t empty_stairs;

// Semaphore that acts as the turnstile to prevent starvation
sem_t stair_manager;

// FUNCTION DEFINITIONS
//Functions to "use the stairs"
void *ascend_stairs(void *customer_threads);
void *descend_stairs(void *customer_threads);

//Semaphore wait and signal functions
void semaphore_wait(sem_t *sem);
void semaphore_signal(sem_t *sem);

// Helper function to reset the direction
void reset_direction(char *direction);

//lightswitch functions
void switch_lock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore);
void switch_unlock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore);

// Helper function to calculate the Turnaround Time and Response Time for each thread
void print_clock_array(void);

// Arrays to track the times threads were created, started, and completed
clock_t thread_create_time[MAX_STORE_CUSTOMERS];
clock_t thread_start_time[MAX_STORE_CUSTOMERS];
clock_t thread_complete_time[MAX_STORE_CUSTOMERS];


int main(void)
{
    printf("Project 2: Customer crossing problem using pThreads and Semaphores\n");
    printf("*****************\n\n");
    fflush(stdout);

    // Arrays to track customer ID and threads
    customer_t customer_data[MAX_STORE_CUSTOMERS];
    pthread_t store_customers[MAX_STORE_CUSTOMERS];

    // set initial value of switch counters to 0.
    up_switch.counter = 0;
    down_switch.counter = 0;

    int errCheck;

    // Initialize the semaphores and check they were initialized properly
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

    // Create the threads
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

        // set creation start_time
        thread_create_time[i] = clock();
    }

    // Join the threads
    for (int i = 0; i < MAX_STORE_CUSTOMERS ; ++i)
    {
        if ((errCheck = pthread_join(store_customers[i], NULL)))
        {
            fprintf(stderr, "error: pthread_join, %d\n", errCheck);
        }
    }

    // Print the Turnaround Time and Response Time for each thread.
    print_clock_array();

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

    // set process start time
    thread_start_time[customer_id] = clock();

    printf("\nDESC: Customer %d would like to descend to the first floor.\n", customer_id);

    // Check stair_manager lock to see if there are others on the stairs.
    semaphore_wait(&stair_manager);
    printf("DESC: Customer %d checking stair_manager lock.\n", customer_id);
    // Check if the direction can/should be set.
    switch_lock(&down_switch, &down_direction, &empty_stairs);
    // Release the stair manager to let the customer enter the stairs
    semaphore_signal(&stair_manager);
    // Call the counting semaphore to use the stairs
    semaphore_wait(&second_floor_gatekeeper);
    // Use the stairs
    printf("**DESC: Customer %d is descending.\n", customer_id);
    sleep(1);
    printf("***DESC: Customer %d reached the first floor.\n", customer_id);
    // Release the counting semaphore to decrement the number of customers on stairs
    semaphore_signal(&second_floor_gatekeeper);
    // Reset the stair direction if alone on the stairs
    switch_unlock(&down_switch, &down_direction, &empty_stairs);
    // Set process completion time
    thread_complete_time[customer_id] = clock();
    pthread_exit(NULL);
}


/*
* Function to enable customer to ascend to the second floor. 
* This uses the reverse logic of the descend stair function.
*/
void *ascend_stairs(void *customer_thread)
{
    customer_t *data = (customer_t *)customer_thread;
    int customer_id = data->id;

    // set process start time
    thread_start_time[customer_id] = clock();

    printf("\nASC: Customer %d would like to ascend to the second floor.\n", customer_id);

    // Check stair_manager lock to see if there are others on the stairs.
    semaphore_wait(&stair_manager);
    // Set the direction if customer is first to the stairs
    switch_lock(&up_switch, &up_direction, &empty_stairs);
    // Release the stair manager to let a customer enter staircase
    semaphore_signal(&stair_manager);
    // Release counting semaphore to decrement the number of customers using the stairs.
    semaphore_wait(&first_floor_gatekeeper);
    // Use the stairs
    printf("**ASC: Customer %d is ascending.\n", customer_id);
    sleep(1);
    printf("***ASC: Customer %d reached the second floor.\n", customer_id);
    // Decrement the number of people using the stairs
    semaphore_signal(&first_floor_gatekeeper);
    // Reset direction if the only person on stairs
    switch_unlock(&up_switch, &up_direction, &empty_stairs);

    // set process completion time
    thread_complete_time[customer_id] = clock();

    pthread_exit(NULL);
}

// Helper functions
/*
 *  Error-checked semaphore wait from provided skeleton
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
 *  Error-checked semaphore signal from provided skeleton
 */
void semaphore_signal(sem_t *sem)
{
    if (sem_post(sem) < 0)
    {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
}


// Lightswitch implementation to set the direction of the staircase.
void switch_lock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore)
{
    semaphore_wait(direction_semaphore);
    direction->counter++;
    if (direction->counter == 1)
    {
        semaphore_wait(locking_semaphore);
        printf("\n****SET DIRECTION****\n");
    }
    semaphore_signal(direction_semaphore);
}

// lightswitch implementation to reset the staircase direction.
void switch_unlock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore)
{
    semaphore_wait(direction_semaphore);
    direction->counter--;
    if (direction->counter == 0)
    {
        semaphore_signal(locking_semaphore);
        printf("****RESET DIRECTION****\n");
    }
    semaphore_signal(direction_semaphore);
}

// Function to print the turnaround time and response time for each thread.
void print_clock_array(void)
{
    printf("************Process Stats**********\n");
    for (int i = 0; i < MAX_STORE_CUSTOMERS; i++)
    {
        printf("Process %d: create: %ld |start: %ld |complete %ld\n", i, thread_create_time[i], thread_start_time[i], thread_complete_time[i]);
        printf("Turnaround Time: %ld", thread_complete_time[i]-thread_create_time[i]);
        printf(" | Response Time: %ld\n\n", thread_start_time[i]-thread_create_time[i]);
    }
}