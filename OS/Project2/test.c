#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <ucontext.h>

#define STACK_SIZE SIGSTKSZ

struct itimerval timer;
ucontext_t contextFoo, contextBar, contextScheduler;
int counter = 0;

void scheduler(int signum){
    setitimer(ITIMER_PROF, &timer, NULL);

    if (counter == 0) {
        counter = 1;
	    swapcontext(&contextScheduler,&contextBar);
    } 
    
    if (counter == 1) {
        counter = 0;
        swapcontext(&contextScheduler, &contextFoo);
    }
}

void* foo() {
    while(1) {
        puts("foo");
    }
}

void* bar() {
    while (1) {
        puts("bar");
    }
}

int main(int argc, char** argv){
	
	if (getcontext(&contextFoo) < 0){
		perror("getcontext");
		exit(1);
	}

	// Allocate space for stack	
	void *stackFoo = malloc(STACK_SIZE);
	
	if (stackFoo == NULL){
		perror("Failed to allocate stack");
		exit(1);
	}
      
	/* Setup context that we are going to use */
	contextFoo.uc_link=NULL;
	contextFoo.uc_stack.ss_sp=stackFoo;
	contextFoo.uc_stack.ss_size=STACK_SIZE;
	contextFoo.uc_stack.ss_flags=0;

	// Make the context to start running at f1withparam()
	makecontext(&contextFoo,(void *)&foo,0);


    if (getcontext(&contextBar) < 0){
		perror("getcontext");
		exit(1);
	}

	// Allocate space for stack	
	void *stackBar = malloc(STACK_SIZE);
	
	if (stackBar == NULL){
		perror("Failed to allocate stack");
		exit(1);
	}
      
	/* Setup context that we are going to use */
	contextBar.uc_link=NULL;
	contextBar.uc_stack.ss_sp=stackBar;
	contextBar.uc_stack.ss_size=STACK_SIZE;
	contextBar.uc_stack.ss_flags=0;
	
	// Make the context to start running at f1withparam()
	makecontext(&contextBar,(void *)&bar,0);


	// Use sigaction to register signal handler
	struct sigaction sa;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &scheduler;
	sigaction (SIGPROF, &sa, NULL);

	// Set up what the timer should reset to after the timer goes off
	timer.it_interval.tv_usec = 0; 
	timer.it_interval.tv_sec = 0;

	// Set up the current timer to go off in 1 second
	// Note: if both of the following values are zero
	//       the timer will not be active, and the timer
	//       will never go off even if you set the interval value
	timer.it_value.tv_usec = 0;
	timer.it_value.tv_sec = 1;

	// Set the timer up (start the timer)
	setitimer(ITIMER_PROF, &timer, NULL);

    setcontext(&contextFoo);

	return 0;

}