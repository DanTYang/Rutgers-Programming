#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct blockage
{
  int valid;
  long int tag;
}block;
typedef struct cacheage
{
  block ** cacheBlocks;
  int cacheBlocksize;
  int numSets;
  int assoc;
  int cacheSize;
  int read;
  int write;
  int miss;
  int hit;
  int policy;
  long int blockOffset;
  long int setIndex;
  long int Index;
}cache;
int powerOfTwo(int num)
{
    if( num<=0 )
    {
      return 0;
    }
    return ((num & (num-1))==0);
}
int logo(int num)
{
    int count = 0;
    while(num > 1)
    {
      count++;
      num = num/2;
    }
    return count;
}
int setAssoc(char* assoc)
{
    if(strcmp(assoc, "direct") == 0)
    {
        return -1;
    }
    else if(strcmp(assoc, "assoc") == 0)
    {
        return -2;
    }
    else if(strchr(assoc, ':') != NULL && strncmp(assoc, "assoc:", 5) == 0)
    {
        return atoi(strchr(assoc, ':') + 1);
    }
    else
    {
      return -3;
    }
}
void read(cache* c, long int memory)
{
  c->Index = (memory >> c->blockOffset) % (1 << c->setIndex);
  if(c->numSets == 1)
  {
    c->Index = 0;
  }
  long int tag = memory>>(c->blockOffset+c->setIndex);
  int counter = 0;
  int wrote = 0;
  for (counter = 0; counter < c->assoc; counter++)
  {
    if(c->cacheBlocks[c->Index][counter].tag == tag)
    {
      wrote = 1;
      if(c->policy == 1)
      {
        int oI = 0;
        for(int i = 0; i < c->assoc; i++)
        {
          if(c->cacheBlocks[c->Index][i].tag == tag)
          {
            oI = i;
          }
        }
        for(int i = oI; i> 0; i--)
        {
          c->cacheBlocks[c->Index][i].tag = c->cacheBlocks[c->Index][i-1].tag;
        }
        c->cacheBlocks[c->Index][0].tag = tag;
      }
      c->hit++;
      break;
    }
  }
  if(wrote == 0)
  {
    for(counter = c->assoc - 1;counter >= 0; counter--)
    {
      if(c->cacheBlocks[c->Index][counter].valid == 0)
      {
        c->cacheBlocks[c->Index][counter].valid = 1;
        c->cacheBlocks[c->Index][counter].tag = tag;
        wrote = 1;
        c->read++;
        c->miss++;
        break;
      }
    }
  }
  if(wrote == 0)
  {
    for(int i = c->assoc - 1;i > 0; i--)
    {
      c->cacheBlocks[c->Index][i].tag = c->cacheBlocks[c->Index][i-1].tag;
    }
    c->cacheBlocks[c->Index][0].tag = tag;
    c->read++;
    c->miss++;
  }
}
void write(cache* c, long int memory)
{
  c->Index = (memory >> c->blockOffset) % (0x1 << c->setIndex);
  if(c->numSets == 1)
  {
    c->Index = 0;
  }
  long int tag = memory>>(c->blockOffset+c->setIndex);
  int counter = 0;
  int wrote = 0;
  for (counter = 0; counter < c->assoc; counter++)
  {
    if(c->cacheBlocks[c->Index][counter].tag == tag)
    {
      wrote = 1;
      if(c->policy == 1)
      {
        int oI = 0;
        for(int i = 0; i < c->assoc; i++)
        {
          if(c->cacheBlocks[c->Index][i].tag == tag)
          {
            oI = i;
          }
        }
        for(int i = oI; i> 0; i--)
        {
          c->cacheBlocks[c->Index][i].tag = c->cacheBlocks[c->Index][i-1].tag;
        }
        c->cacheBlocks[c->Index][0].tag = tag;
      }
      c->hit++;
      c->write++;
      break;
    }
  }
  if(wrote == 0)
  {
    for(counter = c->assoc - 1;counter >= 0; counter--)
    {
      if(c->cacheBlocks[c->Index][counter].valid == 0)
      {
        c->cacheBlocks[c->Index][counter].valid = 1;
        c->cacheBlocks[c->Index][counter].tag = tag;
        wrote = 1;
        c->read++;
        c->miss++;
        c->write++;
        break;
      }
    }
  }
  if(wrote == 0)
  {
    for(int i = c->assoc - 1;i > 0; i--)
    {
      c->cacheBlocks[c->Index][i].tag = c->cacheBlocks[c->Index][i-1].tag;
    }
    c->cacheBlocks[c->Index][0].tag = tag;
    c->read++;
    c->miss++;
    c->write++;
  }
}
cache* intialize(int cacheSize, int cacheBlocksize, int n, int replace)
{
  cache* c = (cache*)malloc(sizeof(cache));
  c->read = 0;
  c->write = 0;
  c->hit = 0;
  c->miss = 0;
  c->policy = replace;
  c->cacheBlocksize = cacheBlocksize;
  c->cacheSize = cacheSize;
  c->Index = 0;
  if(n == -1)
  {
    c->numSets = cacheSize / (cacheBlocksize);
    c->assoc = 1;
    n = 1;
  }
  else if(n == -2)
  {
    c->numSets = 1;
    n = cacheSize / (cacheBlocksize);
    c->assoc = n;
  }
  else
  {
    c->numSets = cacheSize / (cacheBlocksize * n);
    c->assoc = n;
  }
  c->cacheBlocks = (block**)malloc(sizeof(block*)*c->numSets);
  for(int i = 0; i < c->numSets; i++)
  {
    c->cacheBlocks[i] = (block*)malloc(sizeof(block)*n);
    for(int j = 0; j < n; j++)
    {
      c->cacheBlocks[i][j].tag = 0;
      c->cacheBlocks[i][j].valid = 0;
    }
  }
  c->blockOffset = logo(c->cacheBlocksize);
  c->setIndex = logo(c->numSets);
  return c;
}
int main(int argc,char** argv)
{
    if (argc != 6)
    {
        printf("error\n");
        return -1;
    }
    int cacheSize = atoi(argv[1]);
    if(powerOfTwo(cacheSize) == 0 )
    {
        printf("error\n");
        return 0;
    }
    int cacheBlocksize = atoi(argv[2]);
    if(powerOfTwo(cacheBlocksize) == 0 )
    {
        printf("error\n");
        return 0;
    }
    int policyNum = -1;
    char * policy = argv[3];
    if(strcmp(policy,"fifo") == 0)
    {
        policyNum = 0;
    }
    else if(strcmp(policy,"lru")==0)
    {
        policyNum = 1;
    }
    else
    {
        printf("error\n");
        return 0;
    }
    int n = setAssoc(argv[4]);
    if(n==-3)
    {
        printf("error");
        printf("Associativity is incorrect\n");
        return -1;
    }
    else if(n != -1 && n != -2)
    {
        if(powerOfTwo(n) == 0)
        {
            printf("error\n");
            return 0;
        }
    }
    ///////////////////////////////////////////
    FILE* fp = fopen(argv[5], "r");
    if (fp == NULL)
    {
        printf("error\n");
        return 0;
    }
  	char rw;
  	long int adres;
    cache* c = intialize(cacheSize, cacheBlocksize, n, policyNum);
    while(fscanf(fp,"%c 0x%lx\n", &rw, &adres) == 2)
  	{
      if(rw == '#')
      {
        printf("adres: %lx\n", adres);
        break;
      }
      else
      {
        if(rw == 'R')
        {
          read(c, adres);
        }
        else if(rw == 'W')
        {
          write(c, adres);
        }
      }
    }
    printf("Memory reads: %d\n", c->read);
    printf("Memory writes: %d\n", c->write);
    printf("Cache hits: %d\n", c->hit);
    printf("Cache misses: %d\n", c->miss);
    for(int i = 0; i < c->numSets; i++)
    {
      free(c->cacheBlocks[i]);
    }
    free(c->cacheBlocks);
    free(c);
    fclose(fp);
}
