#include <stdlib.h>
#include <stdio.h>

int binarySearch(int ar[], int t, int l, int r)
{
    if (r <= l)
    {
      if(t > ar[l])
      {
        return (l+1);
      }
      else
      {
        return l;
      }
    }
    int mid = (l + r)/2;
    if(t == ar[mid])
    {
        return mid+1;
    }
    if(t > ar[mid])
    {
        return binarySearch(ar, t, mid+1, r);
    }
    return binarySearch(ar, t, l, mid-1);
}

int insertionSort(int ar[], int s, int n)
{
    int location = binarySearch(ar, s, 0, n - 1);
    if(location != 0)
    {
      if(ar[location - 1] == s)
      {
        return 1;
      }
    }
    else
    {
      if(ar[0] == s)
      {
        return 1;
      }
    }
    int temp = ar[location];
    temp = ar[location];
    ar[location] = s;
    s = temp;
    for(int i = location+ 1; i <= n; i++)
    {
      temp = ar[i];
      ar[i] = s;
      s = temp;
    }
    return 0;
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
  int num = 10000;
  int hashsize =0;
  int* hash = (int *)malloc(sizeof(int) * (num));
  int* array = (int *)malloc(sizeof(int) * (num));
  for(int i = 0; i <num; i++)
  {
    char type = 'a';
    fscanf(fp, "%c\t%d\n", &type, &array[i]);
    if(type == 'i')
    {
      if(hashsize != 0)
      {
        int k = insertionSort(hash, array[i], hashsize);
        if(k == 0)
        {
          printf("inserted\n");
          hashsize++;
        }
        else
        {
          printf("duplicate\n");
        }
      }
      else
      {
        hash[hashsize] = array[i];
        printf("inserted\n");
        hashsize++;
      }
    }
    else if(type == 's')
    {
      int location = binarySearch(hash, array[i], 0, hashsize);
      if(location != 0)
      {
        if((hash[location - 1] == array[i]) || (hash[location] == array[i]))
        {
          if(location > hashsize)
          {
              printf("absent\n");
          }
          else if((location == hashsize) && (hash[location] == array[i]))
          {
              printf("absent\n");
          }
          else
          {
            printf("present\n");
          }
        }
        else
        {
          printf("absent\n");
        }
      }
      else
      {
        if((hash[0] == array[i]))
        {
          printf("present\n");
        }
        else
        {
          printf("absent\n");
        }
      }
    }
    if(type == 'a')
    {
      break;
    }
  }
  // printf(">>>>>>-------------------------------------------------\n");
  // printf("array Print:\n");
  // for(int j = 0; j < hashsize; j++)
  // {
  //   printf("hash[%d]: %d\n", j, hash[j]);
  // }
  // printf("-----------------------------------------------<<<<<<<<\n");
}
