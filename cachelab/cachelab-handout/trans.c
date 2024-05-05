/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
/* cache 1KB block 32B */
/* s = 5, E = 1, b = 5 */
/* total 32 group, each block(group) for 8 int */
/* only declare 12 variables of int type */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    if(N == 32 && M == 32)
    {
        int r[8];
        for(int i = 0;i < 32;i += 8)
        {
            for(int j = 0;j < 32; j++)
            {
                /*delete the third cycle*/
               r[0] = A[j][i+0];
               r[1] = A[j][i+1];
               r[2] = A[j][i+2];
               r[3] = A[j][i+3];
               r[4] = A[j][i+4];
               r[5] = A[j][i+5];
               r[6] = A[j][i+6];
               r[7] = A[j][i+7];
               B[i+0][j] = r[0];
               B[i+1][j] = r[1];
               B[i+2][j] = r[2];
               B[i+3][j] = r[3];
               B[i+4][j] = r[4];
               B[i+5][j] = r[5];
               B[i+6][j] = r[6];
               B[i+7][j] = r[7];
            }
        }
    }
    else if(N == 64 && M == 64)
    {
        int r[8];
        for(int i = 0;i < 64; i += 8)
        {
            for(int j = 0;j < 64; j += 8)
            {
                //divide A and B into 8*8 block, each block made of 4 4*4 block, numbered 1 to 4
                /*
                 B B1  B2  A A1  A2
                   B3  B4    A3  A4
                */
                /*A1 --> B1, A2 --> B2*/
                for(int k = 0;k < 4; k++)
                {
                    r[0] = A[k+i][j+0];
                    r[1] = A[k+i][j+1];
                    r[2] = A[k+i][j+2];
                    r[3] = A[k+i][j+3];
                    r[4] = A[k+i][j+4];
                    r[5] = A[k+i][j+5];
                    r[6] = A[k+i][j+6];
                    r[7] = A[k+i][j+7];

                    B[j+0][k+i] = r[0];
                    B[j+1][k+i] = r[1];
                    B[j+2][k+i] = r[2];
                    B[j+3][k+i] = r[3];

                    B[j+0][k+i+4] = r[4];
                    B[j+1][k+i+4] = r[5];
                    B[j+2][k+i+4] = r[6];
                    B[j+3][k+i+4] = r[7];
                }
                /*B2 --> Array, A3 --> B2, Array --> B3*/
                for(int k = 0;k < 4; k++)
                {
                    r[0] = A[i+4][k+j];
                    r[1] = A[i+5][k+j];
                    r[2] = A[i+6][k+j];
                    r[3] = A[i+7][k+j];

                    r[4] = B[k+j][i+4]; 
                    r[5] = B[k+j][i+5];
                    r[6] = B[k+j][i+6];
                    r[7] = B[k+j][i+7];

                    B[k+j][i+4] = r[0];
                    B[k+j][i+5] = r[1];
                    B[k+j][i+6] = r[2];
                    B[k+j][i+7] = r[3];

                    B[k+j+4][i+0] = r[4];
                    B[k+j+4][i+1] = r[5];
                    B[k+j+4][i+2] = r[6];
                    B[k+j+4][i+3] = r[7];
                }
                /*A4 --> Array --> B4*/
                for(int k = 0;k < 4; k += 2)
                {
                    r[0] = A[k+i+4][j+4];
                    r[1] = A[k+i+4][j+5];
                    r[2] = A[k+i+4][j+6];
                    r[3] = A[k+i+4][j+7];

                    r[4] = A[k+i+5][j+4];
                    r[5] = A[k+i+5][j+5];
                    r[6] = A[k+i+5][j+6];
                    r[7] = A[k+i+5][j+7];

                    B[j+4][k+i+4] = r[0];
                    B[j+5][k+i+4] = r[1];
                    B[j+6][k+i+4] = r[2];
                    B[j+7][k+i+4] = r[3];

                    B[j+4][k+i+5] = r[4];
                    B[j+5][k+i+5] = r[5];
                    B[j+6][k+i+5] = r[6];
                    B[j+7][k+i+5] = r[7];
                }
            }
        }
    }
    else if(N == 67 && M == 61)
    {
        int r[8];
        for(int i = 0;i < (M/8)*8; i += 8)
        {
            for(int j = 0;j < N; j++)
            {
                r[0] = A[j][i+0];
                r[1] = A[j][i+1];
                r[2] = A[j][i+2];
                r[3] = A[j][i+3];
                r[4] = A[j][i+4];
                r[5] = A[j][i+5];
                r[6] = A[j][i+6];
                r[7] = A[j][i+7];

                B[i+0][j] = r[0];
                B[i+1][j] = r[1];
                B[i+2][j] = r[2];
                B[i+3][j] = r[3];
                B[i+4][j] = r[4];
                B[i+5][j] = r[5];
                B[i+6][j] = r[6];
                B[i+7][j] = r[7];
            }
        }
        for(int i = 0;i < N; i++)
        {
            r[0] = A[i][56];
            r[1] = A[i][57];
            r[2] = A[i][58];
            r[3] = A[i][59];
            r[4] = A[i][60];

            B[56][i] = r[0];
            B[57][i] = r[1];
            B[58][i] = r[2];
            B[59][i] = r[3];
            B[60][i] = r[4];
        }
    }
    else  
    {
        fprintf(stderr , "Wrong Matrix Arguments!\n");
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

