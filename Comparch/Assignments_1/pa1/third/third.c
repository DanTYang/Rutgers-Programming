#include <stdlib.h>
#include <stdio.h>

int comp(int num, int a)
{
	int temp = 1 << a;
	int test = num & ~temp;
	if(test == num)
	{
		num = num|temp;
	}
	else
	{
		num = test;
	}
	return num;
}

int get(int num, int a)
{
	int temp = 1 << a;
	int test = num & temp;
	if(test > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int set(int num, int a, int b)
{
	int temp = 1 << a;
	num = num & ~temp;
	if(b == 1)
	{
		num = num | temp;
	}
	return num;
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
	char command[5];
	int num1;
	int num2;
  while(fscanf(fp,"%s\t%d\t%d\n", command, &num1, &num2) != EOF)
	{
      if(command[0] == 'g')
      {
       	   int k = get(num, num1);
					 printf("%d\n", k);
      }
      else if(command[0] == 'c')
      {
           num = comp(num, num1);
					 printf("%d\n", num);
      }
      else if(command[0] == 's')
      {
           num = set(num, num1, num2);
					 printf("%d\n", num);
      }
      else
      {
          break;
      }
  }

}
