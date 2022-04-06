# project-2

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