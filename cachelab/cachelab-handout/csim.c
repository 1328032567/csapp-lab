#include "cachelab.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define SIZE 30
//csim -> cache simulator
//Xuan UESTC

void printVerbose();

void printSummary(int hit, int miss, int eviction);

void InitArgv(int *s, int *E, int *b, int *t);
//sample:./csim-ref -v -s 4 -E 1 -b 4 -t traces/trans.trace
int main(int argc, char* argv[])
{
    int mode = 0;
    int s, E, b;
    char *fname;
    FILE* fp;
    // printf("%d\n", argc);
    if(argc == 10)
    {
        mode = 1;
    }
    else if(argc == 9)
    {
        mode = 0;
    }
    else
    {
        fprintf(stderr, "Wrong Arguments!");
        exit(0);
    }
    //Initialize
    s = (int)atoi((char*)argv[2+mode]);
    E = (int)atoi((char*)argv[4+mode]);
    b = (int)atoi((char*)argv[6+mode]);
    fname = argv[8+mode];
    if((fp = fopen( fname, "r")) == NULL)
        fprintf(stderr, "Open file %s failed.", fname);
    // printf("s:%d e:%d b:%d t:%s \n", s, E, b, fname);
    int S = pow(2, s);
    int B = pow(2, b);
    int t = 32 - (b + s);
    char ***cache = (char*** )malloc(sizeof(char) * S * E * 32);
    //Initialize cache
    for(int i = 0;i < S;i++)
    {
        for(int j = 0;j < E;j++)
            cache[i][j][0] = 0;
    }

    
    return 0;
}

void printVerbose();

