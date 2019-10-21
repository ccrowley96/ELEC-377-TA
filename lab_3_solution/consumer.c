/* 	ELEC 377,  Lab 3
 *
 *  consumer.c 
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
    int pid;
    int numProd;
    int charRead;
    int c;

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

//printf("about to read producer count\n");
    getMutex(pid);
    numProd = memptr -> prodCount;
    releaseMutex(pid);
//printf("read producer count\n");

    charRead = 1;
    while(numProd > 0 && charRead){
	charRead = 0;
	while(charRead == 0 && numProd > 0){
//printf("about to attempt to get char from buffer\n");
	    getMutex(pid);
            if (memptr -> count > 0){
		c = memptr->buffer[memptr -> outPtr];
                memptr -> outPtr = (memptr -> outPtr + 1) % BUFFSIZE;
                memptr -> count --;
                charRead  = 1;
            } else {
	           numProd = memptr -> prodCount;
            }
	    releaseMutex(pid);
//printf("charRead = %d\n",charRead);
	}
//printf("got character %c(%d) from buffer\n",c,c);
        putchar(c);
    }

    // no more producers	

    return 0;
}

