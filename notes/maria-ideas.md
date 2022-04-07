# Project Ideas

Solve the problem for the following use cases:

- Two customers are in a line at the same time and are going the same direction
- One customer is upstairs and another is downstairs at the same time and seeks to use stairs
- Two customers are going in the same direction but arrive at separate times. 

THEN SOLVE FOR (U represents queue of customers who are "upstairs" and D represents queue "downstairs":
- U:2 | D: 1
- U:2 | D: 2
- U:4 | D: 8

Things we should consider:
- How should we determine who gets to use the stairs first?
- How many threads should be able to go down the stairs at the same time before blocking? (Maybe this would be a good use for a counting semaphore...)
- Does arrival time impact the scheduling? How so?
- Do we assume customers ascend and descend the stairs at the same rate or is the rate "random?"

NEED TO:
1. Determine how the thread should be set up to determine whether a thread is going "up" or "down"
- From what I see in a previous assignment from 5008, we can create functions that basically say whether something is an up or down thread and have it return NULL. So it could be a customer function like 

```c
// if we want, we can represent the stairs as an array:
int NUM_STAIRS = 13;
int STAIRS[NUM_STAIRS]; // could put an id of the process in the stairs? or does this over-complicate things?


void *up_to_down (void *vargp)
{
    int direction = 0;
    for (int i = NUM_STAIRS; i > 0; i--)
    {
        printf("process: %d is on stair: %d", process_id, i+1);
    }
    sleep(3);
}

void *down_to_up (void *vargp)
{
    int direction = 1;
    for (int i = 0; i < NUM_STAIRS; i++)
    {
        printf("process: %d is on stair: %d", process_id, i+1);
    }
    sleep(3);
}

void *customer_scheduler(queue_t floor_1, queue_t floor_2) {
    // schedules customers here. We may need a queue of customers from each floor.
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


```


2. What other information can and should a thread have and update?


## Specs:
Need to design a C program that:

- prevents deadlock
- prevents starvation
- allows for more than one customer to use the stairs in the same direction in an "efficient" way.

### More details
- Allow more than one customer to ascend or descend the stairs at a particular time without allowing any customers to deadlock on the spiral stairs.
- Use separate threads to represent first-floor and second-floor threads.
- when a customer is "using" the stairs, the thread sleeps for a random period of time. (SHOULD THIS BE RANDOM?)
- Calculate the Turnaround Time and Response time for each customer.

#### Submission instructions
In a readme.md file, include the following:
1. Your group number, and the name of the students in your group 2. A short description of the project.
3. Explain how you implemented the project:
- List the functions you wrote, and the purpose of each function
- Find the average Turnaround time and Response time of the examples you run, and explain using these performance measures how you adjusted your project to make your design “efficient”.
- Explain how we can compile, run and test your code.
- List the contributions of each student in your group

You should place all relevant files to the project in a folder and create a zip file by zipping the folder itself. You should not include object files or executable binary files that are generated during the compilation process. The zip file should be submitted via Canvas. Only one of the team members may submit the work: if one member uploaded the file on Canvas, the other team member will automatically see the same submission. Call your files Project2_group#.