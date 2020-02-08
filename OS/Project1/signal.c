/* signal.c
 *
* Group Members Names and NetIDs:
 *   1.dty15
 *   2.mk1652
 *
 * ILab Machine Tested on:
 * ice
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

/* Part 1 - Step 2 to 4: Do your tricks here
 * Your goal must be to change the stack frame of caller (main function)
 * such that you get to the line after "r2 = *( (int *) 0 )"
 */
void segment_fault_handler(int signum) 
{

    printf("I am slain!\n");

    //We used the gdb to locate the position of signum on the stack when segment_fault_handler is called.
    //Next, we switched info frames in order to locate the position on the stack where the instruction pointer (%rip) was stored.
    //We created a pointer to this position, and we incremented the value by 2 in order to skip past the faulty instruction.

    void* pointer = &signum;
    pointer = pointer + 0xcc;

    *(int*) pointer += 0x2;

}

int main(int argc, char *argv[])
{

    int r2 = 0;

    /* Part 1 - Step 1: Registering signal handler */

    signal(SIGSEGV, segment_fault_handler);

    r2 = *( (int *) 0 ); // This will generate segmentation fault

    printf("I live again!\n");

    return 0;
}
