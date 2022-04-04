
/*
Stairs crossing problem using pThreads and Semaphores
*/

#include <pthread.h>  
#include <semaphore.h> 
#include <stdio.h>    
#include <stdlib.h>
#include <unistd.h>	

#ifndef tNum
// TODO
// #define tNum: which is the maximum number of customers/threads in the system to test
#define tNum 10
// #define maxStairs: how many customer can be on the stairs at the same time
#define maxStairs 13
// you can also define other constants for your "prevent deadlock" or "prevent starvation" algorithm
#endif 


//TODO 
// create a struct as Thread's data. Contains the thread's ID.
typedef struct _tData_t	{int tID; }tData_t;
 
// TODO
// define semaphores you will use
sem_t goingUp, goingDown, mutex;

/* 
 *	the following variables are created to:  
 *	1. prevent deadlock
 *	2. prevent starvation
 *	3. allow more than one customer to use the stairs in the same direction in an “efficient” way
       that you determine
 */
//int xingCnt, xedCnt, toAwaitCnt, toBwaitCnt; 
int xingCnt, xedCnt, toUpwaitCnt, toDownwaitCnt; 


/* 
 *	flag the crossing direction (0 noDir, 1 upDir, 2 downDir)
 * I used an enum instead of int 
 */
//int xingDir; 
enum {noDir, upDir, downDir} xingDir; 


//Customers going toward A (i.e. up) or toward B (i.e. down) respectively
//void *a(void *arg);
//void *b(void *arg);
//I used up and down instead
void *up(void *arg);
void *down(void *arg);

//Semaphore wait and signal functions
void semWait(sem_t *sem);
void semSignal(sem_t *sem);


int main(){

 
	printf("Project 2: Customer crossing problem using pThreads and Semaphores\n");
	printf("*****************\n\n");
	fflush(stdout);

	// TODO
	/*
	 *	initialize your pthread, semaphore and other variables you will need
	 *	please do error check if the program fail to pthread_create()
	 */

	tData_t tData[tNum];
	pthread_t thread[tNum];

	int errCheck;

	if (sem_init(&mutex, 0, (unsigned int)1) < 0
        || sem_init(&goingUp, 0, (unsigned int)1) < 0
        || sem_init(&goingDown, 0, (unsigned int)1) < 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
	}

    printf("Parent Process PID: %d \n", getpid());//parent PID
	fflush(stdout);
	

    // TODO
	/*
	 *	do pthread_create() and call your upstair/downstair func
	 */
	 for (int i = 0; i < tNum; ++i) {

		void *thread_func;

		tData[i].tID = i;

		if (rand()%tNum <=tNum/2) {
			thread_func = up;
		} 
		else{
			thread_func = down;
		}

		sleep(1);
		fflush(stdout);
		sleep(1);
		fflush(stdout);

	if ((errCheck = pthread_create(&thread[i], NULL, thread_func, &tData[i]))) {
            fprintf(stderr, "error: pthread_create, %d\n", errCheck);
            return EXIT_FAILURE;
        }
    }
    
    for (int i = 0; i < tNum; ++i) {
        if ((errCheck = pthread_join(thread[i], NULL))) {
            fprintf(stderr, "error: pthread_join, %d\n", errCheck);
        }
    }

	return EXIT_SUCCESS;
}

/*
* Function for the customer to B level
* down function
*/
void *down(void *arg){


	// TODO
	/*
	 *	process the toB action - down function
	 *	print logs to show customers status (e.g., "customer is Crossing from A to B", 
	 *  "customer to B should wait", “Finished Stairs") 
	 */
	tData_t *data = (tData_t *)arg;

	int tIDdown = data->tID;
	semWait(&mutex);

	printf("****\nThread %d: customer would like to cross from A to B, going down\n", tIDdown);
	fflush(stdout);

	if ((xingDir == downDir || xingDir == noDir) && 
		(xingCnt < 5) &&
		((xedCnt + xingCnt) < 10)){

		printf("Customer is Crossing from A to B, going down\n");
		fflush(stdout);

		xingDir = downDir;
		xingCnt++;

		printf("Signaling Mutex\n");
		fflush(stdout);

		semSignal(&mutex);
	}
	else{
		
		printf("Customer is waiting to go down\n");
		fflush(stdout);
		toDownwaitCnt++;

		printf("Signaling Mutex\n");
		fflush(stdout);
		semWait(&mutex);
		semWait(&goingDown);

		toDownwaitCnt--;
		xingCnt++;
		xingDir = downDir;

		semSignal(&mutex);
	}

	printf ("Using Stairs\n");
	printf("Finished Stairs. Waiting for mutex.\n");
	fflush(stdout);
	semWait(&mutex);
	
	printf("Mutex Passed\n");
	fflush(stdout);
	xedCnt++;
	xingCnt--;

	if (toDownwaitCnt!= 0 &&
		(xingCnt + xedCnt < 10 ||
			(xedCnt + xingCnt>= 10 && toUpwaitCnt == 0))){

		semSignal(&goingDown);
	}

	else if(xingCnt == 0 &&
		toUpwaitCnt !=0 &&
		(toDownwaitCnt == 0 ||
			xedCnt + toDownwaitCnt >= 10)){

		xingDir = upDir;
		xedCnt = 0;
		semSignal(&goingUp);
	}

	else if(xingCnt == 0 &&
		toDownwaitCnt == 0 && 
		toUpwaitCnt == 0){
		xingDir = noDir;
		xedCnt = 0;

		printf("Crossing Direction Reset \n");
		fflush(stdout);
		semSignal(&mutex);
	}

	else{
		printf("Signaling Mutex\n");
		fflush(stdout);
		semSignal(&mutex);
	}

	pthread_exit(NULL);
}


/*
* Function for the customer to A level
*/
void *up(void *arg){


	// TODO
	/*
	 *	process the toA action. 
	 *	print logs to show customers status (e.g., "customer is Crossing from B to A", 
	 *  "customer to A should wait", “Finished Stairs") 
	 */

	tData_t *data = (tData_t *)arg;

	int tIDup = data->tID;
	semWait(&mutex);

	printf("****\nThread %d: customer would like to cross from B to A, going up\n", tIDup);
	fflush(stdout);

	if ((xingDir == upDir || xingDir == noDir) && 
		xingCnt < 5 &&
		(xedCnt + xingCnt < 10)){

		printf("Customer is Crossing from B to A, going up\n");
		fflush(stdout);

		xingDir = upDir;
		xingCnt++;

		printf("Signaling Mutex\n");
		fflush(stdout);

		semSignal(&mutex);
	}
	else{
		printf("A is now supposed to wait to go up\n");
		fflush(stdout);
		toUpwaitCnt++;

		printf("Signaling mutex\n");
		fflush(stdout);
		semWait(&mutex);
		semWait(&goingUp);

		toUpwaitCnt--;
		xingCnt++;
		xingDir = upDir;

		printf("Customer about to go up\n");
		fflush(stdout);
		printf("Signaling Mutex\n");
		semSignal(&mutex);
	}


	printf ("Using Stairs\n");
	printf("Finished Stairs. Waiting for mutex \n");
	fflush(stdout);
	semWait(&mutex);

	printf("Mutex Passed\n");
	fflush(stdout);
	xedCnt++;
	xingCnt--;

	
	if (toUpwaitCnt!= 0 &&
		(xingCnt + xedCnt < 10 ||
			(xedCnt + xingCnt>= 10 && toDownwaitCnt == 0))){

		semSignal(&goingUp);
	}

	else if(xingCnt == 0 &&
		toDownwaitCnt !=0 &&
		(toUpwaitCnt == 0 ||
			xedCnt + toDownwaitCnt >= 10)){
		
		xingDir = downDir;
		xedCnt = 0;
		semSignal(&goingDown);
	}

	else if(xingCnt == 0 &&
		toUpwaitCnt == 0 && 
		toDownwaitCnt == 0){
		printf("Crossing Direction Reset \n");
		fflush(stdout);
		xingDir = noDir;
		xedCnt = 0;

		printf("Signaling Mutex\n");
		fflush(stdout);
		semSignal(&mutex);
	}

	else{
		printf("Signaling Mutex\n");
		fflush(stdout);
		semSignal(&mutex);
	}


	pthread_exit(NULL);
}

// helper functions
/*
 *	Error-checked semaphore wait.
 */
void semWait(sem_t *sem) {
    if (sem_wait(sem) < 0) {
        perror("sem_wait");
        exit(EXIT_FAILURE);
    }
}

/*
 *	Error-checked semaphore signal.
 */
void semSignal(sem_t *sem) {
    if (sem_post(sem) < 0) {
        perror("sem_post");
        exit(EXIT_FAILURE);
    }
}

/*
 *	Expected test result:
 * Please print logs to show the customers' status for testing. Here is an example:
Project 2: Customer crossing problem using pThreads and Semaphores
*****************
Parent Process PID: 86672 
****
Thread 0: About to cross
customer is Crossing from B to A
Crossing Finished
Signaling Mutex
Using Stairs 
Finished Stairs . Waiting for mutex 
Mutex Passed
Crossing Direction Reset 
Signaling Mutex
****
Thread 1: About to cross
customer is Crossing from B to A
Crossing Finished
Signaling Mutex
Using Stairs 
Finished Stairs . Waiting for mutex 
Mutex Passed
Crossing Direction Reset 
Signaling Mutex
****
Thread 2: customer Crossing from A to B
customer is Crossing from A to B
Signaling Mutex
There are 0 customers waiting 
Using Stairs 
The customer will finish crossing in 1 seconds 
Finished Stairs. Waiting for mutex 
Mutex Passed
Crossing Direction Reset 
****
 */

}
