#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>


#define MAX_STORE_CUSTOMERS 50 // let's imagine it's a Daiso
#define MAX_STAIR_USERS 4
#define NUM_STAIRS 13
#define FLOOR_1_SEM
#define FLOOR_2_SEM

typedef struct customer_t 
{
    int id;
} customer_t;

// When STAIR is 0, there is no direction, 1:ascending, 2: desc
enum { up, down, none } stair_direction ;
// array to contain customers


void *descend_stairs(void *customer);
void *ascend_stairs(void *customer);
void semaphore_wait(sem_t *semaphore);
void semaphore_signal(sem_t *semaphore);

// Define global variables to track the users. 
int current_stair_users = 0;
int num_users_crossed = 0;
int num_waiting_to_ascend = 0;
int num_waiting_to_descend = 0;

// semaphore definitions
sem_t ascend;
sem_t descend;
sem_t stair_lock;

int main(void)
{
    customer_t customer_data[MAX_STORE_CUSTOMERS];
    pthread_t store_customers[MAX_STORE_CUSTOMERS];
    
    // create a variable for tracking thread errors
    int error_code;

    // check if there are any semaphore errors (indicated by val -1)
    if (sem_init(&stair_lock, 0, (unsigned int)1) < 0
        || sem_init(&ascend, 0, (unsigned int)1) < 0
        || sem_init(&descend, 0, (unsigned int)1) < 0)
    {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < MAX_STORE_CUSTOMERS; ++i)
    {
        void *customer_direction;

        customer_data[i].id = i;

        if (rand() % MAX_STORE_CUSTOMERS <= MAX_STORE_CUSTOMERS / 2)
        {
            customer_direction = ascend_stairs;
        }
        else {
            customer_direction = descend_stairs;
            sleep(1);
            fflush(stdout);
        }

        if ((error_code = pthread_create(&store_customers[i], NULL, customer_direction, &customer_data[i])))
        {
            fprintf(stderr, "Error in pthread_create, %d\n", error_code);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < MAX_STORE_CUSTOMERS; ++i)
    {
        if (error_code = pthread_join(store_customers[i], NULL))
        {
            fprintf(stderr, "Error in pthread_join: %d\n", error_code);
        }
    }
    return EXIT_SUCCESS;
}

void *descend_stairs(void *customer_number)
{

    customer_t *customer_info = (customer_t *) customer_number;
    int customer_id = customer_info -> id;
    semaphore_wait(&stair_lock);

    printf("DESC customer: %d is on 2nd floor and seeks to descend.\n", customer_id);
    fflush(stdout);

    if ((stair_direction == down || stair_direction == none)
        && current_stair_users < MAX_STAIR_USERS
        && num_users_crossed + current_stair_users < MAX_STAIR_USERS)

    {
        stair_direction = down;
        current_stair_users++;
        semaphore_signal(&stair_lock);
    }
    else
    {
        // is this duplicative? Could we just call the down method again?
        printf("Customer %d is waiting to descend the stairs.\n", customer_id);
        num_waiting_to_descend++;
        semaphore_wait(&stair_lock);
        semaphore_wait(&descend);
        num_waiting_to_descend--;
        current_stair_users++;
        stair_direction = down;
        semaphore_signal(&stair_lock);
    }
    
    int current_stair;

    for (current_stair = NUM_STAIRS; current_stair >= 0; current_stair--)
    {
        if (current_stair == 0)
        {
            printf("DESC customer: %p has reached the first floor.\n\n", (int *)customer_number);
        }
        else
        {
            printf("DESC customer: %p | stair: %d\n", (int *)customer_number, current_stair);
        }
    }
    // do we need a semaphore protection scheme here?
    num_users_crossed++;
    current_stair_users--;

    int total_customers_served = num_users_crossed + current_stair_users;

    if ((num_waiting_to_descend != 0 && total_customers_served < MAX_STAIR_USERS)
        || (total_customers_served >= MAX_STAIR_USERS && num_waiting_to_ascend == 0))
    {
        semaphore_signal(&descend);
    }
    else if (current_stair_users == 0 && num_waiting_to_ascend != 0
        && (num_waiting_to_descend == 0 || num_users_crossed + num_waiting_to_descend >= MAX_STAIR_USERS))
    {
        // do we need to lock these?
        stair_direction = up;
        num_users_crossed = 0;
        semaphore_signal(&ascend);
    }
    else if (current_stair_users == 0 && num_waiting_to_descend == 0 && num_waiting_to_ascend == 0)
    {
        stair_direction = none;
        num_users_crossed = 0;
        printf("Stair direction reset to NONE.\n");
        semaphore_signal(&stair_lock);
    }
    else
    {
        fflush(stdout);
        semaphore_signal(&stair_lock);
    }

    pthread_exit(NULL);
}


void *ascend_stairs(void *customer_number)
{

    customer_t *customer_info = (customer_t *) customer_number;
    int customer_id = customer_info -> id;
    semaphore_wait(&stair_lock);

    printf("ASC customer: %d is on 1st floor and seeks to ascend.\n", customer_id);
    fflush(stdout);

    if ((stair_direction == up || stair_direction == none)
        && current_stair_users < MAX_STAIR_USERS
        && num_users_crossed + current_stair_users < MAX_STAIR_USERS)

    {
        stair_direction = up;
        current_stair_users++;
        semaphore_signal(&stair_lock);
    }
    else
    {
        // is this duplicative? Could we just call the down method again?
        printf("Customer %d is waiting to ascend the stairs.\n", customer_id);
        num_waiting_to_ascend++;
        semaphore_wait(&stair_lock);
        semaphore_wait(&ascend);
        num_waiting_to_ascend--;
        current_stair_users++;
        stair_direction = up;
        semaphore_signal(&stair_lock);
    }
    
    int current_stair;

    for (current_stair = NUM_STAIRS; current_stair >= 0; current_stair--)
    {
        if (current_stair == 0)
        {
            printf("ASC customer: %p has reached the second floor.\n\n", (int *)customer_number);
        }
        else
        {
            printf("ASC customer: %p | stair: %d\n", (int *)customer_number, current_stair);
        }
    }
    // do we need a semaphore protection scheme here?
    num_users_crossed++;
    current_stair_users--;

    int total_customers_served = num_users_crossed + current_stair_users;

    if ((num_waiting_to_ascend != 0 && total_customers_served < MAX_STAIR_USERS)
        || (total_customers_served >= MAX_STAIR_USERS && num_waiting_to_descend == 0))
    {
        semaphore_signal(&ascend);
    }
    else if (current_stair_users == 0 && num_waiting_to_descend != 0
        && (num_waiting_to_ascend == 0 || num_users_crossed + num_waiting_to_ascend >= MAX_STAIR_USERS))
    {
        // do we need to lock these?
        stair_direction = up;
        num_users_crossed = 0;
        semaphore_signal(&descend);
    }
    else if (current_stair_users == 0 && num_waiting_to_ascend == 0 && num_waiting_to_descend == 0)
    {
        stair_direction = none;
        num_users_crossed = 0;
        printf("Stair direction reset to NONE.\n");
        semaphore_signal(&stair_lock);
    }
    else
    {
        fflush(stdout);
        semaphore_signal(&stair_lock);
    }

    pthread_exit(NULL);
}

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
