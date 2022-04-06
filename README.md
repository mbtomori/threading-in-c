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

`void *ascend_stairs(void *customer_thread);`

`void *descend_stairs(void *customer_thread);`



### Project Testing
Explain how you tested your project and list the test cases you used.

### Deadlock and Starvation Handling
Explain how you are guaranteeing that your code is free of deadlock and starvation.

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