# Project-2: Group 6

## Project Specifications:
Need to design a C program that:

- prevents deadlock
- prevents starvation
- allows for more than one customer to use the stairs in the same direction in an "efficient" way.

## Solution Overview
We tried this a number of different ways initially, using global counters and three semaphores but they kept resulting in deadlock.

Upon doing more research, we came across a similar problem in the [Little Book of Semaphores](https://greenteapress.com/semaphores/LittleBookOfSemaphores.pdf). The No-starve unisex bathroom solution (p. 177) utilizes a concept called a light switch (p. 70) to handle a situation where there are competing threads, but more than one of the same thread can use a particular resource (in this case, the stairs.)

In this pattern, the first person to use a resource sets a switch -- which in this problem is the direction of the staircase. These "light switches" each have their own semaphores to set the direction of the staircase, and they also check whether the staircase is currently empty. If it is empty, it can set a new
direction.

We used two counting semaphores to enable multiple customers to use the staircase, one for each direction. 

We also implemented a turnstile `stair_manager` semaphore to allow one customer at a time to stop the flow of the alternate thread.

## Function Overview
- List the functions you wrote, and the purpose of each function.

#### Traversal Functions
The following functions handle the traversal of the staircase for ascending
and descending customers. These functions handle the locking and blocking of 
the opposing direction's semaphores.

`void *ascend_stairs(void *customer_thread);`
- This function handles a customer "ascending" the staircase.

`void *descend_stairs(void *customer_thread);`

- This function handles a customer "descending" the staircase. 

#### Light Switch Functions
The `switch` functions listed below handle determining whether a staircase is empty or whether there are others on it.

`void switch_lock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore);`
- If the thread is the first to the staircase, it can set the direction of the staircase.

`void switch_unlock(lightswitch_t *direction, sem_t *direction_semaphore, sem_t *locking_semaphore);`
- If the thread is the last to exit the staircase, it can reset the direction of the staircase.

#### Turnaround Time and Response Time Print Function
`void print_clock_array(void);`
- This function simply performs printing of data from `clock_t` arrays as well as the calculations to determine the turnaround time and response time. 
- We implemented this function because the process times change each time the code
is run and we figured this was the most accurate way to calculate these metrics.

#### Semaphore Functions
Both functions below are wrapper functions provided in the skeleton code. They 
wrap the `sem_wait()` and `sem_post()` functions from the `semaphore.h` library and handle failures.
- `void semaphore_wait(sem_t *sem)`
- `void semaphore_signal(sem_t *sem)`

## Project Testing
We tested the code manually by viewing the output and ensuring:
1) No more than the `MAX_STAIR_USERS` utilized the staircase at any time (which we could use to prevent starvation).
2) All threads completed (No Deadlock)
3) No threads for the opposite direction went up and down at the same time.

Because we create the thread directions randomly, we weren't able to make the testing very automated.

## Deadlock and Starvation Handling
Explain how you are guaranteeing that your code is free of deadlock and starvation.

Using the reference in the Little Book of Semaphores (p. 70 and p. 177), 
we handle deadlock using the light switch pattern, which allows one thread to enter and set the direction of the stairs if it's the only customer using the stairs. 

A counting semaphore allows a limited number of customers to use the stairs at a time. 

We manage starvation by using counting semaphores to limit the number of 
customers who use the stairwell at any given time in addition to a "turnstile" which only allows one person to enter the protected section until current occupants leave. This decreases performance, but ensures that there will not be starvation, as there are more opportunities to switch the staircase direction.

## Turnaround Time and Response Time
To calculate the Turnaround Time and Response Time, we stored `clock_t` times in three different arrays to track when the thread was created, started, and completed. Then we calculated and printed an individual thread's turnaround time (complete_time - creation_time) and response_time (start_time - creation_time). We tracked the sum of these values and divided them by the number of customers.

We ran a experiments to determine which number of customers using the stairs at the same time seemed better. If we assumed there are 13 steps, we ran the code three times each differing the number of users by 3 to assess what seemed like the best number of concurrent threads. We ran the experiments with 50 customers (threads) in the store. The times measured here are in clocks/second.

#### Average Turnaround Time

| # Concurrent threads | Experiment 1 | Experiment 2 | Experiment 3 | Average |
| -------------------- | ------------ | ------------ | ------------ | ------- |
| 1  (no concurrency)  | 8570.12      | 7578.46      | 8078.92      | 8075.83 |
| 4                    | 6997.24      | 7232.42      | 6661.16      | 6963.60 |
| 7                    | 7925.76      | 8391.50      | 8503.46      | 8273.57 |
| 10                   | 5924.10      | 5842.18      | 8922.04      | 6896.11 |
| 13                   | 7676.28      | 7303.46      | 8789.96      | 7923.23 |

#### Average Response Time

| # Concurrent threads | Experiment 1 | Experiment 2 | Experiment 3 | Average |
| -------------------- | ------------ | ------------ | ------------ | ------- |
| 1  (no concurrency)  | 95.58        | 257.00       | 303.76       | 218.78  |
| 4                    | 164.86       | 132.30       | 203.54       | 166.90  |
| 7                    | 107.18       | 110.20       | 54.56        | 90.65   |
| 10                   | 90.64        | 279.80       | 168.42       | 179.62  |
| 13                   | 111.20       | 75.88        | 185.24       | 124.11  |


Based on these experiments, it's difficult to determine which would be the right number of concurrent users. In reality, we would need to conduct these experiment hundreds or thousands of times to get a good sense of which number
of concurrent stair users would be best. We settled on four due to the somewhat shorter response time and the somewhat shorter turnaround time, but that's easily modified in the code. 

If our code did not have an element of randomness, it would be easier to calculate, but that would not simulate the real world. The fact that we use a random number as a factor in determining which direction to go means that the "customers" can arrive in wildly different configurations that impact these metrics (beyond the hardware and other background processes the computer is using.) What is clear is that the benefits of concurrency are not very clear cut based on these examples.


## Compile and Run Instructions

Compile and run with `make stairs`. The output will print directly to the terminal. 

run `make clean` before re-running.

The code can be tested by reviewing the print statements that are outputted to the terminal.

## Group contributions
Yvette and Maria met twice on our own as well as once with Professor Hamandi to discuss the project. We each wrote down our ideas and implemented drafts using GitHub to collaborate.

Yvette wrote a functional first draft that used the information from the meeting with Professor Hamandi. Unfortunately this solution resulted in deadlock. Maria worked to simplify it and implemented the light switch approach which solved the deadlock problem using the Little Book of Semaphores that Yvette found and suggested.
