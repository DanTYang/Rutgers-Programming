/*
 Reading from a file
 execute with this command: ./Program9 file.txt
 */
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Unexpected number of arguments(%d)\n", argc);
        return 0;
    }
    
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        return 0;
    }
    
    int num;
    char character;
    int r;
    
    r = fscanf(fp, " number: %d, character: %c ", &num, &character);
    if (r == EOF) {
        return 0;
    }
    if (r != 2) {
        return 0;
    }
    
    printf("We retrieved %d and %c\n", num, character);
    
    while (fscanf(fp, " number: %d, character: %c ", &num, &character) != EOF) {
        printf("We retrieved %d and %c\n", num, character);
    }
    
    fclose(fp);
    
    return 0;
}
