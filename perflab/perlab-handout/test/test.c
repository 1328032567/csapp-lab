#include <stdio.h>
typedef struct {
   unsigned short red;
   unsigned short green;
   unsigned short blue;
} pixel;
#define N 8
pixel src[N*N];
pixel dst[N*N];
pixel sum[N][N];
void initSrc(void)
{
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {
            src[i*N+j].red = src[i*N+j].green = src[i*N+j].blue = 12;
        }
    }
}
void print_src(void)
{
    printf("Src:\n");
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {
            printf("%d ", src[i*N+j].red);
        }
        printf("\n");
    }
}
void print_dst(void)
{
    printf("Dst:\n");
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {
            printf("%d ", dst[i*N+j].green);
        }
        printf("\n");
    }
}
void print_sum(void)
{
    printf("Sum:\n");
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {
            printf("%d ", sum[i][j].red);
        }
        printf("\n");
    }
}
void test_dst(void)
{
    for(int i = 0;i < N;i++)
    {
        for(int j = 0;j < N;j++)
        {
            if(dst[i*N+j].red != dst[j*N+i].red)
                printf("Not equal!.\n");
        }
    }
}
void smooth2(int dim, pixel *src, pixel *dst) 
{
    //define and initialize pixel array
    //pixel sum[dim][dim];
    /*
    for(int i = 0;i < dim; i++)
    {
        for(int j = 0;j < dim; j++)
        {
            sum[i][j].red = sum[i][j].green = sum[i][j].blue = 0;
        }
    }
    */

    //up and down adjacent
    //store the first and the last row
    for(int j = 0;j < dim; j++)
    {
        //calculate first line
        //sum[0][j] = src[0][j] + src[1][j];
        //equivalent calculation
        sum[0][j].red = src[j].red + src[dim+j].red;
        sum[0][j].green = src[j].green + src[dim+j].green;
        sum[0][j].blue = src[j].blue + src[dim+j].blue;

        //calculate last line
        //sum[dim-1][j] = src[dim-1][j] + src[dim-2][j];
        //equivalent calculation
        int m = (dim-1)*dim+j;
        int n = (dim-2)*dim+j;
        sum[dim-1][j].red = src[m].red + src[n].red;
        sum[dim-1][j].green = src[m].green + src[n].green;
        sum[dim-1][j].blue = src[m].blue + src[n].blue;
    }

    //up and down adjacent
    //store the rest of the row 
    for(int i = 1; i < dim-1; i++)
    {
        for(int j = 0; j < dim; j++)
        {
            //calculate each line
            //sum[i][j] = src[i-1][j] + src[i-1][j] + src[i-1][j];
            int x = (i-1)*dim + j;
            int y = i*dim + j;
            int z = (i+1)*dim + j;
            sum[i][j].red = src[x].red + src[y].red + src[z].red;
            sum[i][j].green = src[x].green + src[y].green + src[z].green;
            sum[i][j].blue = src[x].blue + src[y].blue + src[z].blue;
        }
    }

    //avg the four corners
    //dst[0][0] = sum[0][0] + sum[0][1]
    //dst[0][dim-1] = sum[0][dim-1] + sum[0][dim-2]
    //dst[dim-1][0] = sum[dim-1][0] + sum[dim-1][1]
    //dst[dim-1][dim-1] = sum[dim-1][dim-1] + sum[dim-1][dim-2]
    {
        //dst[0][0]
        dst[0].red = (unsigned short)(sum[0][0].red + sum[0][1].red) / 4;
        dst[0].green = (unsigned short)(sum[0][0].green + sum[0][1].green) / 4;
        dst[0].blue = (unsigned short)(sum[0][0].blue + sum[0][1].blue) / 4;

        //dst[0][dim-1]
        dst[dim-1].red = (unsigned short)(sum[0][dim-1].red + sum[0][dim-2].red) / 4;
        dst[dim-1].green = (unsigned short)(sum[0][dim-1].green + sum[0][dim-2].green) / 4;
        dst[dim-1].blue = (unsigned short)(sum[0][dim-1].blue + sum[0][dim-2].blue) / 4;

        //dst[dim-1][0]
        dst[(dim-1)*dim].red = (unsigned short)(sum[dim-1][0].red + sum[dim-1][1].red) / 4;
        dst[(dim-1)*dim].green = (unsigned short)(sum[dim-1][0].green + sum[dim-1][1].green) / 4;
        dst[(dim-1)*dim].blue = (unsigned short)(sum[dim-1][0].blue + sum[dim-1][1].blue) / 4;

//WRONG1
        //dst[dim-1][dim-1]
        dst[(dim-1)*(dim+1)].red = (unsigned short)(sum[dim-1][dim-1].red + sum[dim-1][dim-2].red) / 4;
        dst[(dim-1)*(dim+1)].green = (unsigned short)(sum[dim-1][dim-1].green + sum[dim-1][dim-2].green) / 4;
        dst[(dim-1)*(dim+1)].blue = (unsigned short)(sum[dim-1][dim-1].blue + sum[dim-1][dim-2].blue) / 4;
    }
    //avg the four edge lines
    {
        //up line
        for(int i = 1;i < (dim-1);i++)
        {
            //dst[i] = (sum[0][i-1] + sum[0][i] + sum[0][i+1]) / 6
            dst[i].red = (unsigned short)(sum[0][i-1].red + sum[0][i].red + sum[0][i+1].red) / 6;
            dst[i].green = (unsigned short)(sum[0][i-1].green + sum[0][i].green + sum[0][i+1].green) / 6;
            dst[i].blue = (unsigned short)(sum[0][i-1].blue + sum[0][i].blue + sum[0][i+1].blue) / 6;
        }
        //down line
        for(int i = 1;i < (dim-1);i++)
        {
            //dst[(dim-1)*dim+i] = (sum[dim-1][i-1] + sum[dim-1][i] + sum[dim-1][i+1]) / 6
            dst[(dim-1)*dim+i].red = (unsigned short)(sum[dim-1][i-1].red + sum[dim-1][i].red + sum[dim-1][i+1].red) / 6;
            dst[(dim-1)*dim+i].green = (unsigned short)(sum[dim-1][i-1].green + sum[dim-1][i].green + sum[dim-1][i+1].green) / 6;
            dst[(dim-1)*dim+i].blue = (unsigned short)(sum[dim-1][i-1].blue + sum[dim-1][i].blue + sum[dim-1][i+1].blue) / 6;
        }
        //left line
        for(int i = 1;i < (dim-1);i++)
        {
            //dst[i*dim] = (sum[i][0] + sum[i][1]) / 6;
            dst[i*dim].red = (unsigned short)(sum[i][0].red + sum[i][1].red) / 6;
            dst[i*dim].green = (unsigned short)(sum[i][0].green + sum[i][1].green) / 6;
            dst[i*dim].blue = (unsigned short)(sum[i][0].blue + sum[i][1].blue) / 6;
        }
        //right line
        for(int i = 1;i < (dim-1);i++)
        {
            //dst[i*dim+dim-1] = (sum[i][dim-1] + sum[i][dim-2]) / 6;
            dst[i*dim+dim-1].red = (unsigned short)(sum[i][dim-1].red + sum[i][dim-2].red) / 6;
            dst[i*dim+dim-1].green = (unsigned short)(sum[i][dim-1].green + sum[i][dim-2].green) / 6;
            dst[i*dim+dim-1].blue = (unsigned short)(sum[i][dim-1].blue + sum[i][dim-2].blue) / 6;
        }
    }
    //avg for the center of the block
    for(int i = 1;i < dim-1;i++)
    {
        for(int j = 1;j < dim-1;j++)
        {
            //dst[i*dim+j] = (sum[i][j-1] + sum[i][j] + sum[i][j+1]) / 9;
            dst[i*dim+j].red = (unsigned short)(sum[i][j-1].red + sum[i][j].red + sum[i][j+1].red) / 9;
            dst[i*dim+j].green = (unsigned short)(sum[i][j-1].green + sum[i][j].green + sum[i][j+1].green) / 9;
            dst[i*dim+j].blue = (unsigned short)(sum[i][j-1].blue + sum[i][j].blue + sum[i][j+1].blue) / 9;
        }
    }
}
int main()
{
    initSrc();
    //src[0].red = src[0].blue = src[0].green = 4;
    //src[N+1].red = src[N+1].blue = src[N+1].green = 4;
    print_src();
    smooth2(N, src, dst);
    print_dst();
    test_dst();
}