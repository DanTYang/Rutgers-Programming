/*
 Reading from a file
 execute with this command: ./Program9 file2.txt
 */
#include <stdlib.h>
#include <stdio.h>

int** CreateMatrix(int row, int col) {
    int** matrix = (int**)malloc(sizeof(int*) * row);
    int i, j;
    for (i = 0; i < row; i++) {
        matrix[i] = (int*)malloc(sizeof(int) * col);
    }
    
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            matrix[i][j] = 0;
        }
    }
    
    return matrix;
}

void PrintMatrix(int** matrix, int row, int col) {
    int i, j;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void ReadMatrix(int** matrix, int row, int col, FILE* fp) {
    int i, j, num, r;
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++) {
            r = fscanf(fp, " %d ", &num);
            if (r == EOF || r != 1) {
                return;
            }
            matrix[i][j] = num;
        }
    }
}

void Multiply(int** A, int rowA, int colA, int** B, int rowB, int colB, int** result) {
    int i, j, k;
    int rowC = rowA;
    int colC = colB;
    for (i = 0; i < rowC; i++) {
        for (j = 0; j < colC; j++) {
            // Multiply a row i of A with col j of B
            for (k = 0; k < colA; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void FreeMatrix(int** matrix, int row, int col) {
    int i;
    for (i = 0; i < row; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Unexpected number of arguments(%d)\n", argc);
        return 0;
    }
    
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        return 0;
    }
    
    int row, col;
    int r;
    
    r = fscanf(fp, " %d %d ", &row, &col);
    if (r == EOF || r != 2) {
        return 0;
    }
    
    printf("The size of our matrix is %dx%d\n", row, col);
    
    int** A = CreateMatrix(row, col);
    int** B = CreateMatrix(row, col);
    ReadMatrix(A, row, col, fp);
    ReadMatrix(B, row, col, fp);
    
    fclose(fp);
    
    printf("Matrix A:\n");
    PrintMatrix(A, row, col);
    printf("Matrix B:\n");
    PrintMatrix(B, row, col);
    
    int** C = CreateMatrix(row, col);
    Multiply(A, row, col, B, row, col, C);
    
    printf("Matrix C:\n");
    PrintMatrix(C, row, col);
    
    FreeMatrix(A, row, col);
    FreeMatrix(B, row, col);
    FreeMatrix(C, row, col);
    return 0;
}
