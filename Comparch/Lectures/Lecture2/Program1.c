/*
 Hello World program.
 Build this program using the following command:
 gcc Program1.c -Werror -Wall -fsanitize=address -o P1
 */ 

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    printf("Hello World!\n");
    printf("This is how you print a number: %d.\n", 15);
    printf("This is how you print a character: %c.\n", 'A');
    printf("This is how you print a string: %s.\n", "Hello");
    return 0;
}
