/*
 Reading from a file
 Execute with this command: ./Program9 file1.txt
 */
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // This is how you open a file
    if (argc != 2) {
        printf("Unexpected number of arguments(%d)\n", argc);
        return 0;
    }
    
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        return 0;
    }
    
    // How to read a whole line
    char buffer[100];
    char *result;
    result = fgets(buffer, 99, fp);
    
    if (result == NULL) {
        return 0;
    }
    
    printf("%s", buffer);
    
    // How to read a single word
    int r;
    r = fscanf(fp, "%s", buffer);
    if (r == EOF) {
        return 0;
    }
    if (r != 1) {
        return 0;
    }
    
    printf("%s\n", buffer);
    
    // How to keep reading one word at a time
    while(fscanf(fp, "%s", buffer) != EOF) {
        printf("%s\n", buffer);
    }
    
    fclose(fp);

    return 0;
}
