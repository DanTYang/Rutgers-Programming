#include <stdlib.h>
#include <stdio.h>

double **maloc(int N, int K)
{
  double** arr = (double **)malloc(sizeof(double*) * (N));
  for(int i = 0; i < N; i ++)
  {
     arr[i] = (double *)malloc(sizeof(double) * (K));
  }
  return arr;
}
void fre(double** matrix, int a)
{
  for (int i = 0; i < a; i++)
  {
    free(matrix[i]);
  }
  free(matrix);
}
double **mult(double** a, double** b, int aa, int ab, int ba, int bb)
{
  double** result = maloc(aa, bb);
  for(int i = 0; i < aa; i++)
  {
    for(int j = 0; j < bb; j++)
    {
      double sum = 0;
      for(int k = 0; k < ab; k++)
      {
        double mult = a[i][k] * b[k][j];
        sum = sum + (mult);
      }
      result[i][j] = sum;
    }
  }
  return result;
}
double **Transpose(double** matrix, int N, int K)
{
  double ** tran = maloc(K, N);
  for(int i = 0; i < N; i++)
  {
    for(int j = 0; j < K; j++)
    {
      tran[j][i] = matrix[i][j];
    }
  }
  return tran;
}
double **Invert(double** matrix, int N)
{
  double ** inverse = maloc(N, 2 * N);
  for(int i = 0; i < N; i++)
  {
    for(int j = 0; j < N; j++)
    {
      inverse[i][j] = matrix[i][j];
    }
    for(int j = N; j < 2 * N; j++)
    {
      if((j - N) == i)
      {
        inverse[i][j]  = 1;
      }
      else
      {
        inverse[i][j] = 0;
      }
    }
  }
  for(int i = 0; i < N; i++)
  {
    if(inverse[i][i] != 1)
    {
      double div = inverse[i][i];
      for(int j = 0; j < 2 * N; j++)
      {
        inverse[i][j] = inverse[i][j] / div;
      }
    }
    for(int k = 0; k < N; k++)
    {
      double mult = inverse[k][i];
      for(int j = 0; j < 2 * N; j++)
      {
        if(k != i)
        {
            inverse[k][j] =  inverse[k][j] - (mult * inverse[i][j]);
        }
      }
    }
  }
  double ** invert = maloc(N, N);
  for(int i = 0; i < N; i++)
  {
    for(int j = N; j < 2 * N; j++)
    {
      invert[i][j - N] = inverse[i][j];
    }
  }
  return invert;
}
int main(int argc, char** argv)
{
  if (argc != 3) {
      printf("Unexpected number of arguments(%d)\n", argc);
      return 0;
  }
  FILE* fp = fopen(argv[1], "r");
  if (fp == NULL) {
      return 0;
  }
  int K;
  fscanf(fp," %d ",&K);
  int N;
  fscanf(fp," %d ",&N);
  double ** arr = maloc(N, K+1);
  double ** y = maloc(N, 1);
  for(int i = 0; i < N; i++)
	{
    arr[i][0] = 1;
    fscanf(fp,"%lf,", &y[i][0]);
    for(int j = 1; j <= K; j++)
    {
        fscanf(fp,"%lf,", &arr[i][j]);
    }
  }
  K++;
  double **trans = Transpose(arr, N, K);
  double **multi = mult(trans, arr, K, N, N, K);
  double **inverse = Invert(multi, K);
  double **multi2 = mult(inverse, trans, K, K, K, N);
  double **W = mult(multi2, y, K, N, N, 1);
  fre(arr, N);
  fre(trans, K);
  fre(multi, K);
  fre(inverse, K);
  fre(multi2, K);
  free(fp);
  FILE* fp2 = fopen(argv[2], "r");
  if (fp2 == NULL)
  {
    return 0;
  }
  int T = 0;
  fscanf(fp2, " %d ", &T);
  double **testdata = maloc(T, K);
  for(int i = 0; i < T; i++)
  {
    testdata[i][0] = 1;
    for(int j = 1; j < K; j++)
    {
      fscanf(fp2,"%lf,", &testdata[i][j]);
    }
  }
  double **FinalResult = mult(testdata, W, T, K, K, 1);
  for(int i = 0; i < T; i++)
  {
    for(int j = 0; j < 1; j++)
    {
      printf("%0.0lf ",FinalResult[i][j]);
    }
    printf("\n");
  }
}
