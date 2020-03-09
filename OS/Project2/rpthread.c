// File:	rpthread.c

// List all group member's name:
// username of iLab:
// iLab Server:

#include "rpthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE

//Timer variable
struct itimerval timer;

//Context for the scheduler
ucontext_t schedulerContext;

//Runqueues for STCF and MLFQ
runQueue* headSTCF;
runQueue headMLFQ[8];

//The current thread that is running
tcb* currentThread;

//For finished threads, used to store return value when joins
threadReturn* finishedList;

//Used to initialize the sigaction and timer
int started = 0;

/* create a new thread */
int rpthread_create(rpthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg) {
	// Create Thread Control Block
    // Create and initialize the context of this thread
    // Allocate space of stack for this thread to run
    // after everything is all set, push this thread int

    // YOUR CODE HERE
	
	tcb* newThread = malloc(sizeof(tcb*));
	newThread -> tid = &thread;
	newThread -> threadStatus = 0; 
	newThread -> timeElapsed = 0;

	ucontext_t newContext;

	if (getcontext(&newContext) < 0) {
		perror("Error with getcontext");
		exit(1);
	}
	
	void* newStack = malloc(STACK_SIZE);
	
	if (newStack == NULL){
		perror("Failed to allocate stack");
		exit(1);
	}
      
	newContext.uc_link = NULL;
	newContext.uc_stack.ss_sp = newStack;
	newContext.uc_stack.ss_size = STACK_SIZE;
	newContext.uc_stack.ss_flags = 0;

	makecontext(&newContext, (void *)&function, 1, (void*) arg);

	newThread -> threadContext = newContext;

	enqueue(newThread);

	if (!started) {
		started = 1;
		initialize();
	}

    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int rpthread_yield() {
	// Change thread state from Running to Ready
	// Save context of this thread to its thread control block
	// switch from thread context to scheduler context

	// YOUR CODE HERE

	swapcontext(&(currentThread -> threadContext), &schedulerContext);

	return 0;
};

/* terminate a thread */
void rpthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE

	if (value_ptr != NULL) {
		threadReturn* newFinishedThread = malloc(sizeof(threadReturn*));

		newFinishedThread -> tid = currentThread -> tid;
		newFinishedThread -> value = value_ptr;

		if (finishedList == NULL)
			newFinishedThread -> next = NULL;
		else
			newFinishedThread -> next = finishedList;

		finishedList = newFinishedThread;
	}

	free(currentThread);

	currentThread = NULL;

	schedule();
};


/* Wait for thread termination */
int rpthread_join(rpthread_t thread, void **value_ptr) {
	// Wait for a specific thread to terminate
	// De-allocate any dynamic memory created by the joining thread
  
	// YOUR CODE HERE

	threadReturn* previous = NULL;
	threadReturn* current = finishedList;

	while (current != NULL) {
		if (current -> tid == thread) {
			value_ptr = &(current -> value);

			return 0;
		}

		previous = current;
		current = current -> next;
	}

	return 0;
};

/* initialize the mutex lock */
int rpthread_mutex_init(rpthread_mutex_t *mutex, 
                          const pthread_mutexattr_t *mutexattr) {
	//Initialize data structures for this mutex

	// YOUR CODE HERE



	return 0;
};

/* aquire the mutex lock */
int rpthread_mutex_lock(rpthread_mutex_t *mutex) {
	// use the built-in test-and-set atomic function to test the mutex
	// When the mutex is acquired successfully, enter the critical section
	// If acquiring mutex fails, push current thread into block list and 
	// context switch to the scheduler thread

	// YOUR CODE HERE



	return 0;
};

/* release the mutex lock */
int rpthread_mutex_unlock(rpthread_mutex_t *mutex) {
	// Release mutex and make it available again. 
	// Put threads in block list to run queue 
	// so that they could compete for mutex later.

	// YOUR CODE HERE



	return 0;
};


/* destroy the mutex */
int rpthread_mutex_destroy(rpthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in rpthread_mutex_init

	// YOUR CODE HERE



	return 0;
};

/* scheduler */
static void schedule() {
	// Every time when timer interrup happens, your thread library 
	// should be contexted switched from thread context to this 
	// schedule function

	// Invoke different actual scheduling algorithms
	// according to policy (STCF or MLFQ)

	// if (sched == STCF)
	//		sched_stcf();
	// else if (sched == MLFQ)
	// 		sched_mlfq();

	// YOUR CODE HERE

	setitimer(ITIMER_PROF, &timer, NULL);

	// schedule policy
	#ifndef MLFQ
		// Choose STCF
		sched_stcf();
	#else 
		// Choose MLFQ
		sched_mlfq();
	#endif

}

/* Preemptive SJF (STCF) scheduling algorithm */
static void sched_stcf() {
	// Your own implementation of STCF
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE
}

// Feel free to add any other functions you need

// YOUR CODE HERE

void initialize() {
	struct sigaction signal;
	memset (&signal, 0, sizeof (signal));
	signal.sa_handler = &schedule;
	sigaction (SIGPROF, &signal, NULL);

	timer.it_interval.tv_usec = 0; 
	timer.it_interval.tv_sec = 0;
	timer.it_value.tv_usec = 0;
	timer.it_value.tv_sec = 1;

	schedule();
}

void enqueue(tcb* threadControlBlock) {
	if (head == NULL) {
		head = threadControlBlock;
	} else {
		//Do other shit
	}
}

