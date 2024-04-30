#include "cachelab.h"
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#define SIZE 50
//define cache_line struct
typedef struct{
    int valid;
    int tag;
    int stamp;
}Cache_line;
//define cache struct
typedef struct{
    int S;
    int E;
    int B;
    Cache_line** line;
}Cache;

Cache cache = {0, 0, 0, NULL};
int hit = 0;
int miss = 0;
int evic = 0;
int verbose = 0;

void print_help(void);
void Init_Cache(int s, int E, int b);
void Get_Oper(int s, int E, int b, char *fname);
void Update_Cache(int op_tag, int op_s);
int Get_Index(int op_tag, int op_s);
int Find_Empty(int op_s);
int main(int argc, char* argv[])
{
    char opt;
    int s, E, b;
    char fname[SIZE]; 
    while((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (opt) 
        {
            case 'h':
                print_help();
                exit(0);
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = atoi(optarg);
                break; 
            case 'E':
                E = atoi(optarg);
                break; 
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                strcpy(fname, optarg);
                break;
            default:
                fprintf(stderr, "Wrong arguments!\n");
                print_help();
                exit(-1);
        }
    }
    Init_Cache(s, E, b);
    Get_Oper(s, E, b, fname);



    printSummary(0, 0, 0);
    return 0;
}

void print_help(void)
{
    puts("Usage: ./csim-ref [-hv] -s <s> -E <E> -b <b> -t <tracefile>       ");
	puts("• -h: Optional help flag that prints usage info                   ");
	puts("• -v: Optional verbose flag that displays trace info              ");
	puts("• -s <s>: Number of set index bits (S = 2s is the number of sets) ");
	puts("• -E <E>: Associativity (number of lines per set)                 ");
	puts("• -b <b>: Number of block bits (B = 2b is the block size)         ");
	puts("• -t <tracefile>: Name of the valgrind trace to replay            ");
}

void Init_Cache(int s, int E, int b)
{
    int S = 1 << s;
    int B = 1 << b;
    cache.S = S;
    cache.E = E;
    cache.B = B;
    cache.line = (Cache_line**)malloc(sizeof(Cache_line*) * S);
    for(int i = 0;i < S; i++)
    {
        cache.line[i] = (Cache_line *)malloc(sizeof(Cache_line) * E);
        for(int j = 0;j < E;j++)
        {
            cache.line[i][j].valid = 0;
            cache.line[i][j].tag = -1;
            cache.line[i][j].stamp = 0;
        }
    }
}
void Get_Oper(int s, int E, int b, char *fname)
{
    //open the file
    FILE *fp;
    if((fp = fopen(fname, "r")) == NULL)
    {
        fprintf(stderr, "Usage:Can't open file %s", fname);
        exit(-2);
    }
    char operate;
    unsigned address;
    int size;
    while((fscanf(fp, " %c %x,%d", &operate, &address, &size)) != EOF)
    {
        int op_tag = address >> (s + b);
        //get op_s by mask code low s bits are 1 other bits are 0
        int op_s = (address >> b) & ((1 << s) - 1);
        //printf("tag:%d group:%d \n", op_tag, op_s);
        switch(operate)
        {
            case 'M':
                Update_Cache(op_tag, op_s);
            case 'L':
            case 'S':
                Update_Cache(op_tag, op_s);
                break;
        }
    }
    fclose(fp);
    return ;
}
void Update_Cache(int op_tag, int op_s)
{
    int index = Get_Index(op_tag, op_s);
    if(index == -1)
    {
        miss++;
        if(verbose == 1)
        {
            fprintf(stdout, "%s ", "miss");
        }
        int empty = Find_Empty(op_s);
        if(empty == -1)
        {

        }
        else 
        {
        
        }
    }
    else 
    {
        hit++;
    }
}
int Get_Index(int op_tag, int op_s)
{
    for(int i = 0;i < cache.E;i++)
    {
        if((cache.line[op_s][i].valid == 1) && (cache.line[op_s][i].tag == op_tag))
            return 1;
    }
    return -1;
}
int Find_Empty(int op_s)
{
    for(int i = 0; i < cache.E;i++)
    {
        if(cache.line[op_s][i].valid == 0)
            return i;
    }
    return -1;
}