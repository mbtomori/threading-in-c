# Project-2: Group 6

## Specs:
Need to design a C program that:

- prevents deadlock
- prevents starvation
- allows for more than one customer to use the stairs in the same direction in an "efficient" way.

### Solution Overview
We tried this a number of different ways initially, using global counters and three semaphores but they kept resulting in deadlock.

Upon doing more research, we came across a similar problem in the [Little Book of Semaphores](https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf). The No-starve unisex bathroom solution (p. 177) utilizes a concept called a light switch (p. 70) to handle a situation where there are competing threads, but more than one of the same thread can use a particular resource (in this case, the stairs.)

In this pattern, the first person to use a resource sets a switch -- which in this problem is the direction of the staircase. These "light switches" each have their own semaphores to set the direction of the staircase, and they also check whether the staircase is currently empty. If it is empty, it can set a new
direction.

We used two counting semaphores to enable multiple customers to use the staircase, one for each direction. 

We also implemented a turnstile `stair_manager` semaphore to allow one customer at a time to stop the flow of the alternate thread.

### Function Overview
- List the functions you wrote, and the purpose of each function.

#### Traversal Functions
The following functions handle the traversal of the staircase for ascending
and descending customers. These functions handle the locking and blocking of 
the opposing direction's semaphores.

`void *ascend_stairs(void *customer_thread);`
This function handles a customer "ascending" the staircase.

`void *descend_stairs(void *customer_thread);`
This function handles a customer "descending" the staircase. 

#### Light Switch Functions
The `switch` functions listed below handle determining whether a staircase is empty or whether there are others on it.
`void switch_lock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore);`
If the thread is the first to the staircase, it can set the direction of the staircase.

`void switch_unlock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore);`
If the thread is the last to exit the staircase, it can reset the direction of the staircase.

#### Turnaround Time and Response Time Print Function
`void print_clock_array(void);`
This function simply performs printing of data from `clock_t` arrays as well as the calculations to determine the turnaround time and response time. 

We implemented this function because the process times change each time the code
is run and we figured this was the most accurate way to calculate these metrics.

#### Semaphore Functions
Both functions below are wrapper functions provided in the skeleton code. They 
wrap the `sem_wait()` and `sem_post()` functions from the `semaphore.h` library and handle failures.
- `void semaphore_wait(sem_t *sem)`
- `void semaphore_signal(sem_t *sem)`

### Project Testing
We tested the code manually by viewing the output and ensuring:
1) No more than the `MAX_STAIR_USERS` utilized the staircase at any time (which we could use to prevent starvation).
2) All threads completed (No Deadlock)
3) No threads for the opposite direction went up and down at the same time.

Because we create the thread directions randomly, we weren't able to make the testing very automated.

### Deadlock and Starvation Handling
Explain how you are guaranteeing that your code is free of deadlock and starvation.

Using the reference in the Little Book of Semaphores (p. 70 and p. 177), 
we handle deadlock using the light switch pattern, which allows one thread to enter and set the direction of the stairs if it's the only customer using the stairs. 

A counting semaphore allows a limited number of customers to use the stairs at a time. 

We manage starvation by using counting semaphores to limit the number of 
customers who use the stairwell at any given time in addition to a "turnstile" which only allows one person to enter the protected section until current occupants leave. This decreases performance, but ensures that there will not be starvation, as there are more opportunities to switch the staircase direction.

### Turnaround Time and Response Time
- Find the average Turnaround time and Response time of the examples you run, and explain using these performance measures how you adjusted your project to make your design “efficient”.

### More details
- Allow more than one customer to ascend or descend the stairs at a particular time without allowing any customers to deadlock on the spiral stairs.
- Use separate threads to represent first-floor and second-floor threads.
- when a customer is "using" the stairs, the thread sleeps for a random period of time. (SHOULD THIS BE RANDOM?)
- Calculate the Turnaround Time and Response time for each customer.

### Compile and Run Instructions

Compile and run with `make stairs`. The output will print directly to the terminal.

run `make clean` before re-running.

### Group contributions
- List the contributions of each student in your group

You should place all relevant files to the project in a folder and create a zip file by zipping the folder itself. You should not include object files or executable binary files that are generated during the compilation process. The zip file should be submitted via Canvas. Only one of the team members may submit the work: if one member uploaded the file on Canvas, the other team member will automatically see the same submission. Call your files Project2_group#.