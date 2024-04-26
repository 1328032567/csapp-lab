#include "cachelab.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#define SIZE 50
//csim -> cache simulator
//Xuan UESTC

void printVerbose();
void InitArgv(int *s, int *E, int *b, int *t);
int hextodec(char* str, int n);
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
    char ***cache = (char*** )malloc(sizeof(char) * S * E * (t + 1));

    //Initialize cache
    for(int i = 0;i < S;i++)
    {
        for(int j = 0;j < E;j++)
            *(cache + i*E*(t+1) + j*(t+1))= 0;
    }

    //handle the stdin command
    char cmd[SIZE];
    //initialize hit, miss, eviction
    int hit = 0;
    int miss = 0;
    int evic = 0;
    while((fgets(cmd, SIZE, fp)) != NULL && cmd[0] != '\n')
    {
        char* ad;
        int length = 0;
        if(cmd[0] == ' ')
        {
            //get the address command part
            for(int i = 3;i < SIZE;i++)
            {
                if(cmd[i] == ',')
                {
                    length = i - 3;
                    break;
                }
            }

            ad = cmd + 3;
            int at, as;
            int num = hextodec(ad, length-1);
            printf("num:%d\n", num);
            //handle the t and s
            if(4*length <= b)
            {
                at = as = 0;
            }
            else if(4*length > b && 4*length <= (s+b))
            {
                at = 0;
                as = num >> b;
            }
            else if(4*length > (s+b) && 4*length <= 32)
            {
                num >>= b;
                int tmp = num;
                at = num >> s;
                as = tmp % (as<<s);
            }
            printf("t:%d s:%d \n", at, as);
        }
    }
    printSummary(1, 2, 3);
    return 0;
}

void printVerbose();

int hextodec(char* str, int n)
{
    int sum = 0;
    for (int i = n; i >= 0; --i)
		if (str[i] >= '0' && str[i] <= '9')
			sum += (str[i] - '0') * pow(16, n - i);
		else if (str[i] >= 'A' && str[i] <= 'F')
			sum += (str[i] - 'A' + 10) * pow(16,n - i);
		else
			sum += (str[i] - 'a' + 10) * pow(16, n - i);
    return sum;
}

