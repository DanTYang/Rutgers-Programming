/*
 Arrays revisited
 */
#include <stdlib.h>
#include <stdio.h>

void arrayExample1() {
    // Arrays are a contiguous memory.
    int array[10];
    printf("The size of an array is: %lu bytes\n", sizeof(int));
    
    int i;
    for (i = 0; i < 10; i++) {
        array[i] = (i + 1) * 3;
    }
    
    for (i = 0; i < 10; i++) {
        printf("array[%d] = %d\n", i, array[i]);
    }
    
    int* arrayPointer = array;
    printf("array[0] = %d\n", *arrayPointer);
    printf("arrayPointer[0] = %d\n", arrayPointer[0]);
    
    arrayPointer = arrayPointer + 1;
    printf("*arrayPointer = %d\n", *arrayPointer);
    
    printf("arrayPointer = %p\n", arrayPointer);
    printf("   &array[1] = %p\n", &array[1]);
    
    // How do we dynamically create an array?
    int size = 20;
    int* array2 = (int *)malloc(sizeof(int) * size);
    
    for (i = 0; i < 20; i++) {
        array2[i] = i * 2;
    }
    
    for (i = 0; i < 20; i++) {
        printf("array2[%d] = %d\n", i, array2[i]);
    }
    
    for (i = 0; i < 20; i++) {
        *(array2 + i) = i * 5;
    }
    
    for (i = 0; i < 20; i++) {
        printf("*(array2 + i) = %d\n", *(array2 + i));
    }
    
    free(array2);
}


void PassedArray(int* arr, int size) {
    if (arr == NULL) return;
    arr[0] = 25;
    return;
}

int **Create3x3Matrix() {
    int i, j;
    
    // Allocating a matrix: 2 dimensional array (3x3 integer matrix):
    int **matrix = (int**)malloc(sizeof(int*) * 3);
    for (i = 0; i < 3; i++) {
        matrix[i] = (int*)malloc(sizeof(int) * 3);
    }
    
    // Now you can refer to each of the elements using matrix[i][j];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            matrix[i][j] = i * 3 + j;
        }
    }
    
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
    return matrix;
}

void Free3x3Matrix(int** matrix) {
    int i;
    for (i = 0; i < 3; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main(int argc, char** argv) {
    int * array = (int*)malloc(sizeof(int) * 10);
    int i;
    for (i = 0; i < 10; i++) {
        array[i] = i;
    }
    
    free(array);
    
    PassedArray(array, 10);
    
    return 0;
}
