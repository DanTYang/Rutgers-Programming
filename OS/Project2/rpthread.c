// File:	rpthread.c

// List all group member's name: Martin Kong, Daniel Yang
// username of iLab: mk1652, dty15
// iLab Server: ilab1

#include "rpthread.h"

// INITAILIZE ALL YOUR VARIABLES HERE
// YOUR CODE HERE

//Used to indicate if current thread is finished running
int isFinished = 0;

//Used to create the main context
int isFirstTime = 1;

//For the tid in pthread_create
int tidCounter = 0;

//Timer variable
struct itimerval timer;
struct itimerval timerOff;

//Context for the scheduler
ucontext_t schedulerContext, mainContext;
tcb* mainThread = NULL;

//Runqueues for STCF and MLFQ
runQueue* headSTCF;
runQueue* headMLFQ[8];

//The current thread that is running
tcb* currentThread = NULL;

//For finished threads, used to store return value when joins
finishedList* finishedThreads = NULL;

//Used to store blocked threads
blockedList* blockedThreads = NULL;

//For resetting all threads in the MLFQ (once the timer reaches 20)
int resetMLFQTimer = 0;

static void schedule();
static void sched_stcf();
static void sched_mlfq();
void initialize();
void enqueueSTCF(tcb* threadControlBlock);
tcb* dequeueSTCF();
void enqueueMLFQ(tcb* threadControlBlock);
tcb* dequeueMLFQ();
void resetMLFQ();
void threadRunner(void *(*function)(void*), void *arg);
void signalHandler(int signum);

/* create a new thread */
int rpthread_create(rpthread_t * thread, pthread_attr_t * attr, void *(*function)(void*), void * arg) {
	// Create Thread Control Block
    // Create and initialize the context of this thread
    // Allocate space of stack for this thread to run
    // after everything is all set, push this thread int

    // YOUR CODE HERE

	if (isFirstTime) {
		//Creates main thread and main context
		mainThread = malloc(sizeof(tcb));
		mainThread -> tid = tidCounter++;
		mainThread -> threadStatus = 0; 
		mainThread -> timeElapsed = 0;

		getcontext(&mainContext);

		mainThread -> threadContext = mainContext;

		#ifndef MLFQ
			enqueueSTCF(mainThread);
			dequeueSTCF(mainThread);
		#else 
			enqueueMLFQ(mainThread);
			dequeueMLFQ(mainThread);
		#endif

		//Creates scheduler context
		getcontext(&schedulerContext);
	
		void* schedulerStack = malloc(STACK_SIZE);
      
		schedulerContext.uc_link = NULL;
		schedulerContext.uc_stack.ss_sp = schedulerStack;
		schedulerContext.uc_stack.ss_size = STACK_SIZE;
		schedulerContext.uc_stack.ss_flags = 0;

		makecontext(&schedulerContext, (void*)&schedule, 0);
	}

	tcb* newThread = malloc(sizeof(tcb));
	newThread -> tid = tidCounter++;
	*thread = newThread -> tid; 
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

	makecontext(&newContext, (void*)&threadRunner, 2, function, arg);

	newThread -> threadContext = newContext;

	#ifndef MLFQ
		enqueueSTCF(newThread);
	#else 
		enqueueMLFQ(newThread);
	#endif

	if (isFirstTime) {
		isFirstTime = 0;

		struct sigaction signal;
		memset(&signal, 0, sizeof(signal));
		signal.sa_handler = &signalHandler;
		sigaction (SIGPROF, &signal, NULL);

		timer.it_interval.tv_usec = 0; 
		timer.it_interval.tv_sec = 0;
		timer.it_value.tv_usec = 10;
		timer.it_value.tv_sec = 0;

		timerOff.it_interval.tv_usec = 0; 
		timerOff.it_interval.tv_sec = 0;
		timerOff.it_value.tv_usec = 0;
		timerOff.it_value.tv_sec = 0;

		currentThread = mainThread;

		setitimer(ITIMER_PROF, &timer, NULL);
	}

    return 0;
};

/* give CPU possession to other user-level threads voluntarily */
int rpthread_yield() {
	// Change thread state from Running to Ready
	// Save context of this thread to its thread control block
	// switch from thread context to scheduler context

	// YOUR CODE HERE

	setitimer(ITIMER_PROF, &timerOff, NULL);

	swapcontext(&(currentThread -> threadContext), &schedulerContext);

	return 0;
};

/* terminate a thread */
void rpthread_exit(void *value_ptr) {
	// Deallocated any dynamic memory created when starting this thread

	// YOUR CODE HERE

	setitimer(ITIMER_PROF, &timerOff, NULL);

	finishedList* finishedThread = malloc(sizeof(finishedList));

	finishedThread -> tid = currentThread -> tid;
	finishedThread -> value = value_ptr;

	if (finishedThreads == NULL)
		finishedThread -> next = NULL;
	else
		finishedThread -> next = finishedThreads;

	finishedThreads = finishedThread;

	isFinished = 1;

	setcontext(&schedulerContext);
};


/* Wait for thread termination */
int rpthread_join(rpthread_t thread, void **value_ptr) {
	// Wait for a specific thread to terminate
	// De-allocate any dynamic memory created by the joining thread
  
	// YOUR CODE HERE
	
	while (1) {
		setitimer(ITIMER_PROF, &timerOff, NULL);

		finishedList* previous = NULL;
		finishedList* current = finishedThreads;

		while (current != NULL) {
			if (current -> tid == thread) {
				if (current -> value != NULL)
					value_ptr = &(current -> value);

				if (previous == NULL)
					finishedThreads = current -> next;
				else 
					previous -> next = current -> next;

				free(current);

				return 0;
			}

			previous = current;
			current = current -> next;
		}

		swapcontext(&(currentThread -> threadContext), &schedulerContext);
	}

	return 0;
};

/* initialize the mutex lock */
int rpthread_mutex_init(rpthread_mutex_t *mutex, 
                          const pthread_mutexattr_t *mutexattr) {
	//Initialize data structures for this mutex

	// YOUR CODE HERE

	mutex = malloc(sizeof(rpthread_mutex_t));
	mutex -> isLocked = 0;
	mutex -> tid = -1;

	return 0;
};

/* aquire the mutex lock */
int rpthread_mutex_lock(rpthread_mutex_t *mutex) {
	// use the built-in test-and-set atomic function to test the mutex
	// When the mutex is acquired successfully, enter the critical section
	// If acquiring mutex fails, push current thread into block list and 
	// context switch to the scheduler thread

	// YOUR CODE HERE

	if (mutex -> isLocked == 1) {
		setitimer(ITIMER_PROF, &timerOff, NULL);

		blockedList* blockedThread = malloc(sizeof(blockedList));
		blockedThread -> threadControlBlock = currentThread;
		blockedThread -> threadControlBlock -> threadStatus = 2;
		blockedThread -> tid = mutex -> tid;

		currentThread = NULL;

		if (blockedThreads == NULL)
			blockedThread -> next = NULL;
		else
			blockedThread -> next = blockedThreads;

		blockedThreads = blockedThread;

		swapcontext(&(blockedThread -> threadControlBlock -> threadContext), &schedulerContext);
	}

	mutex -> isLocked = 1;
	mutex -> tid = currentThread -> tid;

	return 0;
};

/* release the mutex lock */
int rpthread_mutex_unlock(rpthread_mutex_t *mutex) {
	// Release mutex and make it available again. 
	// Put threads in block list to run queue 
	// so that they could compete for mutex later.

	// YOUR CODE HERE

	if (mutex -> isLocked == 1 && mutex -> tid == currentThread -> tid) {
		mutex -> isLocked = 0;
		mutex -> tid = -1;

		blockedList* current = blockedThreads;
		blockedList* previous = NULL;
		while (current != NULL) {
			if (current -> tid == currentThread -> tid) {
				current -> threadControlBlock -> threadStatus = 0;

				#ifndef MLFQ
					enqueueSTCF(current -> threadControlBlock);
				#else 
					enqueueMLFQ(current -> threadControlBlock);
				#endif

				if (previous == NULL) {
					current = current -> next;
				} else {
					previous -> next = current -> next;
					blockedList* remove = current;
					current = current -> next;

					//free(remove);
				}
			} else {
				previous = current;
				current = current -> next;
			}
		}

		return 0;
	}

	return -1;
};


/* destroy the mutex */
int rpthread_mutex_destroy(rpthread_mutex_t *mutex) {
	// Deallocate dynamic memory created in rpthread_mutex_init

	// YOUR CODE HERE

	//free(mutex);

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

	while(1) {
		setitimer(ITIMER_PROF, &timerOff, NULL);

		if (isFinished) {
			isFinished = 0;

			free((currentThread -> threadContext).uc_stack.ss_sp);
			free(currentThread);
			currentThread = NULL;
		}

		if (currentThread != NULL) {
			currentThread -> threadStatus = 0;
			currentThread -> timeElapsed = currentThread -> timeElapsed + 1;

			enqueueSTCF(currentThread);
		}

		currentThread = dequeueSTCF();

		currentThread -> threadStatus = 1;

		setitimer(ITIMER_PROF, &timer, NULL);

		//printf("Current Thread ID: %d, Time Elapsed: %d\n", currentThread -> tid, currentThread -> timeElapsed);

		swapcontext(&schedulerContext, &(currentThread -> threadContext));
	}
}

/* Preemptive MLFQ scheduling algorithm */
static void sched_mlfq() {
	// Your own implementation of MLFQ
	// (feel free to modify arguments and return types)

	// YOUR CODE HERE

	while (1) {
		setitimer(ITIMER_PROF, &timerOff, NULL);
	
		if (isFinished) {
			isFinished = 0;

			free((currentThread -> threadContext).uc_stack.ss_sp);
			free(currentThread);
			currentThread = NULL;
		}

		if (currentThread != NULL) {
			currentThread -> threadStatus = 0;
			if (currentThread -> timeElapsed != 7)
				currentThread -> timeElapsed = currentThread -> timeElapsed + 1;

			enqueueMLFQ(currentThread);
		}
		
		resetMLFQTimer++;
		if (resetMLFQTimer == 20) {
			resetMLFQTimer = 0;
			resetMLFQ();
		}

		currentThread = dequeueMLFQ();
		
		currentThread -> threadStatus = 1;

		setitimer(ITIMER_PROF, &timer, NULL);

		//printf("Current Thread ID: %d, Time Elapsed: %d\n", currentThread -> tid, currentThread -> timeElapsed);

		swapcontext(&schedulerContext, &(currentThread -> threadContext));
	}
}

// Feel free to add any other functions you need

// YOUR CODE HERE

void threadRunner(void *(*function)(void*), void *arg) {
    function(arg);
}

void signalHandler(int signum) {
	swapcontext(&(currentThread -> threadContext), &(schedulerContext));
}

void enqueueSTCF(tcb* threadControlBlock) {
	runQueue* newRunQueue = malloc(sizeof(runQueue));

	newRunQueue -> threadControlBlock = threadControlBlock;

	if (headSTCF == NULL) {
		newRunQueue -> next = NULL;
		
		headSTCF = newRunQueue;
	} else {
		runQueue* previous = NULL;
		runQueue* current = headSTCF;

		int timeElapsed = threadControlBlock -> timeElapsed;

		while (current != NULL) {
			if (current -> threadControlBlock -> timeElapsed > timeElapsed) {
				if (previous == NULL) {
					newRunQueue -> next = headSTCF;

					headSTCF = newRunQueue;
				} else {
					previous -> next = newRunQueue;
					newRunQueue -> next = current;
				}

				return;
			}

			previous = current;
			current = current -> next;
		}

		previous -> next = newRunQueue;
		newRunQueue -> next = NULL;
	}
}

tcb* dequeueSTCF() {
	if (headSTCF == NULL)
		return NULL;

	tcb* runningThread = headSTCF -> threadControlBlock;
	headSTCF = headSTCF -> next;

	return runningThread;
}

void enqueueMLFQ(tcb* threadControlBlock) {
	runQueue* newRunQueue = malloc(sizeof(runQueue));

	newRunQueue -> threadControlBlock = threadControlBlock;
	newRunQueue -> next = NULL;

	int position = newRunQueue -> threadControlBlock -> timeElapsed;

	if (headMLFQ[position] == NULL) {

		//printf("Thread %d queued at position %d\n", threadControlBlock -> tid, position);
		
		headMLFQ[position] = newRunQueue;
	} else {
		runQueue* current = headMLFQ[position];
		runQueue* previous = NULL;

		while (current != NULL) {
			previous = current;
			current = current -> next;
		}

		//printf("Thread %d queued at position %d\n", threadControlBlock -> tid, position);

		previous -> next = newRunQueue;
	}
}

tcb* dequeueMLFQ() {
	int i;
	for (i = 0; i < 8; i++) {
		if (headMLFQ[i] != NULL) {
			tcb* runningThread = headMLFQ[i] -> threadControlBlock;

			//printf("Thread %d dequeued at position %d\n", runningThread -> tid, runningThread -> timeElapsed);

			headMLFQ[i] = headMLFQ[i] -> next;

			return runningThread;
		}
	}

	return NULL;
}

void resetMLFQ() {
	int i;
	for (i = 1; i < 8; i++) {
		runQueue* current = headMLFQ[i];
		if (current != NULL) {
			while (current -> next != NULL)
				current = current -> next;
			
			if (headMLFQ[0] != NULL) {
				current -> next = headMLFQ[0];
				headMLFQ[0] = headMLFQ[i];
			} else {
				headMLFQ[0] = headMLFQ[i];
			}
		}

		headMLFQ[i] = NULL;
	}
}

