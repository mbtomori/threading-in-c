
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
#define MAX_STORE_CUSTOMERS  15
// #define MAX_STAIR_USERS: how many customer can be on the stairs at the same time
#define MAX_STAIR_USERS 13
// you can also define other constants for your "prevent deadlock" or "prevent starvation" algorithm
#endif 


//TODO 
// create a struct as Thread's data. Contains the thread's ID.
typedef struct _customer_t
{
    int tID; 
} customer_t;
 
// TODO
// define semaphores you will use
sem_t ascend;
sem_t descend;
sem_t stair_lock;

/* 
 *  the following variables are created to:  
 *  1. prevent deadlock
 *  2. prevent starvation
 *  3. allow more than one customer to use the stairs in the same direction in an “efficient” way
       that you determine
 */

int current_stair_users, num_users_crossed, number_waiting_to_ascend, num_waiting_to_descend; 


/* 
 * I used an enum instead of int 
 */
enum {none, up, down} stair_direction; 


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

    customer_t customer_data[MAX_STORE_CUSTOMERS ];
    pthread_t thread[MAX_STORE_CUSTOMERS ];

    int errCheck;

    if (sem_init(&stair_lock, 0, (unsigned int)1) < 0
        || sem_init(&ascend, 0, (unsigned int)1) < 0
        || sem_init(&descend, 0, (unsigned int)1) < 0)
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

        customer_data[i].tID = i;

        if (rand() % MAX_STORE_CUSTOMERS  <= MAX_STORE_CUSTOMERS / 2)
        {
            thread_func = ascend_stairs;
        } 
        else{
            thread_func = descend_stairs;
        }

        // Why was this in here twice?
        // sleep(1);
        // fflush(stdout);
        sleep(1);
        fflush(stdout);

    if ((errCheck = pthread_create(&thread[i], NULL, thread_func, &customer_data[i])))
    {
            fprintf(stderr, "error: pthread_create, %d\n", errCheck);
            return EXIT_FAILURE;
        }
    }
    
    for (int i = 0; i < MAX_STORE_CUSTOMERS ; ++i)
    {
        if ((errCheck = pthread_join(thread[i], NULL)))
        {
            fprintf(stderr, "error: pthread_join, %d\n", errCheck);
        }
    }

    return EXIT_SUCCESS;
}

/*
* Function for the customer to descend to lower level
*/
void *descend_stairs(void *arg)
{


    // TODO
    /*
     *  process the toB action - down function
     *  print logs to show customers status (e.g., "customer is Crossing from A to B", 
     *  "customer to B should wait", “Finished Stairs") 
     */
    customer_t *data = (customer_t *)arg;

    int tIDdown = data->tID;
    semaphore_wait(&stair_lock);

    printf("****\nThread %d: customer would like to cross from A to B, going down\n", tIDdown);
    fflush(stdout);

    if ((stair_direction == down || stair_direction == none) && 
        (current_stair_users < 5) &&
        ((num_users_crossed + current_stair_users) < 10))
    {

        printf("Customer is Crossing from A to B, going down\n");
        fflush(stdout);

        stair_direction = down;
        current_stair_users++;

        printf("Signaling stair_lock\n");
        fflush(stdout);

        semaphore_signal(&stair_lock);
    }
    // is this duplicative? Could we just call the down method again?
    else
    {
        printf("Customer is waiting to go down\n");
        fflush(stdout);
        num_waiting_to_descend++;

        printf("Signaling stair_lock\n");
        fflush(stdout);
        semaphore_wait(&stair_lock);
        semaphore_wait(&descend);

        num_waiting_to_descend--;
        current_stair_users++;
        stair_direction = down;

        semaphore_signal(&stair_lock);
    }

    printf ("Using Stairs\n");
    printf("Finished Stairs. Waiting for stair_lock.\n");
    fflush(stdout);
    semaphore_wait(&stair_lock);
    
    printf("stair_lock Passed\n");
    fflush(stdout);

    // is this duplicative? Could we just call the down method again?
    num_users_crossed++;
    current_stair_users--;

    if (num_waiting_to_descend!= 0 &&
        (current_stair_users + num_users_crossed < 10 ||
            (num_users_crossed + current_stair_users>= 10 && number_waiting_to_ascend == 0)))
    {

        semaphore_signal(&descend);
    }

    else if(current_stair_users == 0 &&
        number_waiting_to_ascend !=0 &&
        (num_waiting_to_descend == 0 ||
            num_users_crossed + num_waiting_to_descend >= 10))
    {

        // Do we need to lock something before calling this?
        stair_direction = up;
        num_users_crossed = 0;
        semaphore_signal(&ascend);
    }

    else if(current_stair_users == 0 &&
        num_waiting_to_descend == 0 && 
        number_waiting_to_ascend == 0)
    {
        stair_direction = none;
        num_users_crossed = 0;

        printf("Crossing Direction Reset \n");
        fflush(stdout);
        semaphore_signal(&stair_lock);
    }

    else{
        printf("Signaling stair_lock\n");
        fflush(stdout);
        semaphore_signal(&stair_lock);
    }

    pthread_exit(NULL);
}


/*
* Function for the customer to 2nd level
*/
void *ascend_stairs(void *arg)
{
    customer_t *data = (customer_t *)arg;

    int tIDup = data->tID;
    semaphore_wait(&stair_lock);

    printf("****\nThread %d: customer would like to cross from B to A, going up\n", tIDup);
    fflush(stdout);

    if ((stair_direction == up || stair_direction == none) && 
        current_stair_users < 5 &&
        (num_users_crossed + current_stair_users < 10))
    {

        printf("Customer is Crossing from B to A, going up\n");
        fflush(stdout);

        stair_direction = up;
        current_stair_users++;

        printf("Signaling stair_lock\n");
        fflush(stdout);

        semaphore_signal(&stair_lock);
    }
    else
    {
        printf("A is now supposed to wait to go up\n");
        fflush(stdout);
        number_waiting_to_ascend++;

        printf("Signaling stair_lock\n");
        fflush(stdout);
        semaphore_wait(&stair_lock);
        semaphore_wait(&ascend);

        number_waiting_to_ascend--;
        current_stair_users++;
        stair_direction = up;

        printf("Customer about to go up\n");
        fflush(stdout);
        printf("Signaling stair_lock\n");
        semaphore_signal(&stair_lock);
    }


    printf ("Using Stairs\n");
    printf("Finished Stairs. Waiting for stair_lock \n");
    fflush(stdout);
    semaphore_wait(&stair_lock);

    printf("stair_lock Passed\n");
    fflush(stdout);
    num_users_crossed++;
    current_stair_users--;

    
    if (number_waiting_to_ascend!= 0 &&
        (current_stair_users + num_users_crossed < 10 ||
            (num_users_crossed + current_stair_users>= 10 && num_waiting_to_descend == 0)))
    {

        semaphore_signal(&ascend);
    }

    else if(current_stair_users == 0 &&
        num_waiting_to_descend !=0 &&
        (number_waiting_to_ascend == 0 ||
            num_users_crossed + num_waiting_to_descend >= 10))
    {
        
        stair_direction = down;
        num_users_crossed = 0;
        semaphore_signal(&descend);
    }

    else if(current_stair_users == 0 &&
        number_waiting_to_ascend == 0 && 
        num_waiting_to_descend == 0)
    {
        printf("Crossing Direction Reset \n");
        fflush(stdout);
        stair_direction = none;
        num_users_crossed = 0;

        printf("Signaling stair_lock\n");
        fflush(stdout);
        semaphore_signal(&stair_lock);
    }

    else{
        printf("Signaling stair_lock\n");
        fflush(stdout);
        semaphore_signal(&stair_lock);
    }

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