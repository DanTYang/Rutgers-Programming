/*
 Pointers and Addresses
 */
#include <stdlib.h>
#include <stdio.h>

void PassByValueFunction(int);
void PassByReferenceFunction(int *);

int main(int argc, char** argv) {

    int i = 4;
    
    printf("i = %d\n", i);
    printf("The address of i = %p\n", &i);
    
    // You can save address into a variable!
    int *address = &i;
    printf("The address of i = %p\n", address);
    
    // And we call this variable pointer!
    int *pointer = &i;
    printf("Pointer of i = %p\n", pointer);
    
    // How to access value of i using pointer?
    int j = *pointer + 1;
    printf("j = %d\n", j);
    
    // How to change value of i using pointer?
    *pointer = 25;
    printf("i = %d\n", i);
    
    // Pointer used for pass by reference argument
    i = 4;
    PassByValueFunction(i);
    printf("Pass by Value result: i = %d\n", i);
    
    i = 4;
    PassByReferenceFunction(&i);
    printf("Pass by reference result: i = %d\n", i);
    
}

void PassByValueFunction(int n) {
    n = n + 1;
    return;
}

void PassByReferenceFunction(int * p) {
    *p = *p + 1;
    return;
}
