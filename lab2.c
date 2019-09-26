/*	ELEC 377,  Lab 2 Solution
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>

#define BUFFSIZE 1024               //arbitrary size big enough to hold output
#define NRTHREAD_LOC 0xc038b3a8     //hex address of nr_threads


char returnBuff[BUFFSIZE];           //buffer used hold output of the sprintf (our output)

//global vars that are preserved between calls.

static struct task_struct * theTask, *lastTask;
struct proc_dir_entry * proc_entry;

// read procedure -- used by kernel to retrieve
//  contents of /proc file

int my_read(char *page, char **start, off_t fpos, int blen, int *eof, void*data){

    int *nrthrd = (int*)NRTHREAD_LOC;  //pointer to nr_threads
    int numChars;  //numChars is the number of characters we have written to the buffer.
   
 
    // first time the process reads our file, fpos is zero, all subsequent
    // reads is is more than zero.
    if(fpos == 0)
    {
        // first read of the file
        // start by printing process and thread information
        numChars = sprintf(returnBuff,"Number of running processes: %d\n", nr_running);
        numChars += sprintf(returnBuff+numChars, "Number of threads: %d\n", *nrthrd);
        
        // print headings for process information.
        numChars += sprintf(returnBuff+numChars, "PID\tUID\tNICE\n");

        // initialization for printing information of each process
        // start with init_task
        theTask = &init_task;
        // in some rare circumstatnces, init_task has a pid of zero.
        while(theTask->pid == 0)theTask=theTask->next_task;
        // circular linked list, remember the starting point.
        lastTask=theTask;
        // print the first element of the list.
        numChars +=  sprintf(returnBuff+numChars,"%d\t%d\t%d\n", theTask->pid,theTask->uid,theTask->nice);
        // advance to next element.
	do
    	    theTask=theTask->next_task;
        while(theTask->pid == 0);
        
    } else {
        // second or later read of the file.
        // add information about the current process to the buffer.
        if (lastTask == theTask) {
            // if the last read
            *start = page;
            *eof = 1;
            return 0;
        }
        
        //numChars =  sprintf(returnBuff,"%d\t%d\t%d\n", theTask->pid,(int)(theTask->times).tms_utime,(int)(theTask->times).tms_stime);
        numChars =  sprintf(returnBuff,"%d\t%d\t%d\n", theTask->pid,theTask->uid,theTask->nice);
        
        // find next non-zero task (but don't skip initial task
	do
    	    theTask=theTask->next_task;
        while(theTask->pid == 0);

    }

    // in case the buffer is not long enough.
    if(numChars > blen)
    {
        numChars = blen;
    }
    strncpy(page,returnBuff,numChars);
	 
    *start = page;    //points to start of next write on buffer
    *eof = 1;	     //says all the data can fit in the page
   
    return numChars;  //returns output size
}

// called when the module is loaded
// - create entry in /proc
int init_module()
{
    // struct proc_dir_entry * proc_entry;
   
    proc_entry = create_proc_entry("lab2",0444,NULL); //creates a new /proc file named "lab2"
   
    if(proc_entry == NULL)
	return 1;    //return failure if the kernel was unable to create a proc entry.
   
    // register read proc callback routine
    proc_entry->read_proc = my_read;
    
    // return success
    return 0;
}

// called when module is unloaded
// - remove entry in .proc
void cleanup_module()
{
    remove_proc_entry("lab2",NULL);  //removes "lab2" file from /proc
}
