/*
 Factorial program
 */
#include <stdlib.h>
#include <stdio.h>

int factorial(int);

int main(int argc, char** argv) {
    printf("Hello this is a factorial program\n");
    
    if (argc != 2) {
        printf("Wrong number of arguments\n");
        return 0;
    }
    
    int num = atoi(argv[1]);
    int result = factorial(num);
    printf("Factorial(%d) = %d\n", num, result);
}

int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n-1);
}
