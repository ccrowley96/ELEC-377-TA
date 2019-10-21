/* 	ELEC 377, Lab 3
 *
 * common.c contains routines to be used from both the  producer, and the  consumer
 *   Mutual Exclusion routines will be here
 */

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <asm/system.h>

#include "common.h"

#define FALSE 0
#define TRUE 1

static struct shared *sharedptr;

void mutexInit(struct shared *memptr){
        // initialize the only mutex once, from the producer... 
    sharedptr = memptr;
}

int test_and_set(int * lock){
    return __cmpxchg(lock,0,1,4);
}

void getMutex(short pid){
	// this should not return until it has mutual exclusion. Note that many versions of 
	// this will probobly be running at the same time.
    int key; 
  
    sharedptr -> waiting[pid] = 1;
    key = 1;
    while(sharedptr->waiting[pid] && key){
       key = test_and_set(&(sharedptr->lock));
    }
    sharedptr -> waiting[pid] = 0;

}

void releaseMutex(short pid){
    // pass the key to the next process

    int j = (pid + 1) % NUMPROCS;
    while (j != pid && !sharedptr->waiting[j]){
	    j = (j + 1) % NUMPROCS;
    } 
    if (pid == j)
       sharedptr->lock = 0;
    else
       sharedptr->waiting[j] = 0;
}
