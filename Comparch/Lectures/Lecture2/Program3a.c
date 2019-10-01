/*
 Arrays, Character arrays, and strings.
 */
#include <stdlib.h>
#include <stdio.h>

void CreateArray() {
    int array[10];
    
    for (int i = 0; i < 10; i++) {
        printf("array[%d] = %d\n", i, array[i]);
    }
    
    for (int i = 0; i < 20; i++) {
        array[i] = i;
    }
    
    printf("\n\n");
    
    for (int i = 0; i < 10; i++) {
        printf("array[%d] = %d\n", i, array[i]);
    }
}

int main(int argc, char** argv) {
    char str1[10] = {'H', 'e', 'l', 'l', 'o', ' ', 'J', 'a', 'y', '\0'};
    char* str2 = "Nice day today";
    
    char concat[25];
    
    for (int i = 0; i < 9; i++) {
        concat[i] = str1[i];
    }
    concat[9] = ' ';
    
    for (int i = 0; i < 14; i++) {
        concat[10 + i] = str2[i];
    }
    concat[24] = '\0';
    
    printf("%s\n", concat);
    
}
