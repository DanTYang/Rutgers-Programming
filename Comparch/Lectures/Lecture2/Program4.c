/*
 Find the first 100 prime numbers.
 */
#include <stdlib.h>
#include <stdio.h>

int IsPrime(int num) {
    for (int i = 2; i < num; i++) {
        if ((num / i) * i == num) {
            return 0;
        }
    }
    return 1;
}

int main_old(int argc, char** argv) {
    
    if (argc != 2) {
        printf("We require 2 arguments. Provided with %d arguments\n", argc);
        return 0;
    }

    char * numInStrFormat = argv[1];
    int num = atoi(numInStrFormat);
    
    printf("Number is: %d\n", num);
    
    if (IsPrime(num)) {
        printf("This number is a prime\n");
    }
    else {
        printf("This number is not a prime\n");
    }
    
    return 0;
}

int main(int argc, char** argv) {
    
    int primeCount = 0;
    int currentNumber = 2;
    while (primeCount < 100) {
        if (IsPrime(currentNumber)) {
            primeCount++;
            printf("Prime #%d: %d\n", primeCount, currentNumber);
        }
        
        currentNumber++;
    }
}
