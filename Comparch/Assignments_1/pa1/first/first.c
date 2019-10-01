#include <stdlib.h>
#include <stdio.h>

void sort(int * array, int size, int type)
{
  for(int i = 0; i < size; i++)
  {
    for(int j = i; j < size; j++)
    {
      int temp = 0;
      if(type == 0)
      {
        if(array[i] > array[j])
        {
          temp = array[i];
          array[i] = array[j];
          array[j] = temp;
        }
      }
      else
      {
        if(array[i] < array[j])
        {
          temp = array[i];
          array[i] = array[j];
          array[j] = temp;
        }
      }
      temp = 0;
    }
  }
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Unexpected number of arguments(%d)\n", argc);
        return 0;
    }
    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL) {
        return 0;
    }
    int num;
    fscanf(fp," %d ",&num);
    int* array = (int *)malloc(sizeof(int) * (num));
    int oddCount = 0;
    int evenCount = 0;
    for(int i = 0; i <num; i++)
    {
      fscanf(fp, " %d ", &array[i]);
      if(array[i] % 2 != 0)
      {
        oddCount++;
      }
      else
      {
        evenCount++;
      }
    }
    int* even = (int *)malloc(sizeof(int) * (evenCount));
    int* odd = (int *)malloc(sizeof(int) * (oddCount));
    int e = 0;
    int o = 0;
    for(int i = 0; i < num; i++)
    {
      if(array[i] % 2 == 0)
      {
        even[e] = array[i];
        e++;
      }
      else
      {
        odd[o] = array[i];
        o++;
      }
    }
    sort(odd, oddCount, 0);
    sort(even, evenCount, 1);
    for(int i = 0; i < oddCount; i++)
    {
      printf("%d\t", odd[i]);
    }
    for(int i = 0; i < evenCount; i++)
    {
      printf("%d\t", even[i]);
    }
}
