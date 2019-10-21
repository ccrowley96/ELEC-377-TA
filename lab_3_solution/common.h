/*
 *  common.h - Common definiton of the shared
 *     memory segment and prototypes for semaphore functions
 * 
 *  Created by Thomas Dean
 *  Copyright 2005 Queen's University.
 *
 */

#define NUMPROCS 5
#define MEMSIZE 200
#define BUFFSIZE 5

struct shared {
   // synchronization variables
   int waiting[NUMPROCS];
   int lock;

   // buffer variables
   char buffer[BUFFSIZE];
   int inPtr;
   int outPtr;
   int count;
   int prodCount;
};


void mutexInit(struct shared *memptr);
void getMutex(short pid);
void releaseMutex(short pid);
