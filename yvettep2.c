/*
Stairs crossing problem using pThreads and Semaphores
*/

#include <pthread.h>  
#include <semaphore.h> 
#include <stdio.h>    
#include <stdlib.h>
#include <unistd.h>	


#define MAX_CUSTOMERS 1
#define maxStairs 13


typedef enum direction 
{
    DIRECTION_UP = 0;
    DIRECTION_DOWN,
    DIRECTION_MAX
}enumDirection_t;

typedef enum error
{
    ERROR = -1,
    SUCCESS,
    ERROR_MAX
}error_t;

typedef struct customer
{
    enumDirection_t directionFlag;
    int sleep;

}
