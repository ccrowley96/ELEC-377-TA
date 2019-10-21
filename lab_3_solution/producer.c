/* 	ELEC 377,  Lab 3
 *
 *  producer.c 
 * - read input
 */

#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <errno.h>

#include "common.h"

#define FALSE 0
#define TRUE 1

int main (int argc, char *argv[]){

    // initialize the shared memory, load in the initial array's, spawn the worker
    // processes.

    key_t   key;
    struct shared    *memptr;
    int shmid;
    int c,stored;
    int pid;

    if (argc != 2) {
	fprintf(stderr,"Usage: %s processid\n",argv[0]);
	exit(1);
    }
    // no error checking...
    pid = atoi(argv[1]); 
    if (pid < 0 || pid >= NUMPROCS){
	fprintf(stderr,"pid between 0 and %d\n",NUMPROCS-1);
	exit(1);
    }

    /*	 Shared memory init 	*/
    key = ftok(".", 'S');
    if((shmid = shmget(key, MEMSIZE, IPC_CREAT|0666)) == -1 ){
	if( (shmid = shmget(key, MEMSIZE, 0)) == -1){
	    fprintf(stderr,"Error allocating shared memory. \n");
	    exit(1);
	}
    }

    // now map the region..
    if((int)(memptr = (struct shared *) shmat(shmid, 0, 0)) == -1){
	fprintf(stderr,"Couldn't map the memory into our process space.\n");
	exit(1);
    }
	
    mutexInit(memptr);

    // put your code here..
//printf("about to increment producer count\n");
    getMutex(pid);
    memptr -> prodCount++;
    releaseMutex(pid);
//printf("producer count incremented\n");

    while((c = getchar())!=EOF){
//printf("about to add the character %c(%d) to buffer\n",c,c);
	stored = 0;
	while(stored == 0){
//printf("about to get mutex for adding character to buffer\n");
	    getMutex(pid);
	    if (memptr -> count < BUFFSIZE){
	       memptr->buffer[memptr ->inPtr] = c;
               memptr-> inPtr = (memptr -> inPtr + 1)%BUFFSIZE;
	       memptr->count++;
	       stored = 1;
	    }
	    releaseMutex(pid);
//printf("released mutext for adding char to buffer\n");
	}
    }

//printf("about to decrement producer count\n");
    getMutex(pid);
    memptr -> prodCount--;
    releaseMutex(pid);
//printf("producer count decremented\n");

    return 0;
}


