/*
 Create a program that grabs a string of text and then calculates the total length of the string.
 */
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    
    if (argc != 2) {
        printf("We require 2 arguments. Provided with %d arguments\n", argc);
        return 0;
    }
    
    printf("Received: %s\n", argv[1]);
    
    // string str = argv[1];
    char * str = argv[1];
    
    int charCount = 0;
    int charIndex = 0;
    while (str[charIndex] != '\0') {
        charCount++;
        charIndex++;
    }
    
    printf("The string has %d characters\n", charCount);
    
    str[charIndex] = ' ';
    
    printf("%s\n", str);
    
    return 0;
}
