/* syscall.c
 *
* Group Members Names and NetIDs:
 *   1.dty15
 *   2.mk1652
 *
 * ILab Machine Tested on:
 * ice
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>                                                                                
#include <sys/syscall.h>
#include <sys/types.h>


double avg_time = 0;

int main(int argc, char *argv[]) {

    /* Implement Code Here */

    // Remember to place your final calculated average time
    // per system call into the avg_time variable

    struct timeval start, end;
    uid_t uid;

    int i;
    for(i = 0; i < 5000000; i++)
    {
        gettimeofday(&start, NULL);

        uid = syscall(SYS_getuid);

        gettimeofday(&end, NULL);
	    long seconds = (end.tv_sec - start.tv_sec);
	    long timeTaken = ((seconds * 1000000)  + end.tv_usec) - (start.tv_usec);

        avg_time += timeTaken;
    }
    
	avg_time /= 5000000;

    printf("Average time per system call is %0.4f microseconds\n", avg_time);

    return 0;
}
