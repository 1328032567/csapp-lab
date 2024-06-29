/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/* 
 * Please fill in the following team struct 
 */
team_t team = {
    "Xuan",              /* Team name */

    "xuan",     /* First member full name */
    "xuan@uestc.edu",  /* First member email address */

    "",                   /* Second member full name (leave blank if none) */
    ""                    /* Second member email addr (leave blank if none) */
};

/***************
 * ROTATE KERNEL
 ***************/

/******************************************************
 * Your different versions of the rotate kernel go here
 ******************************************************/

/* 
 * naive_rotate - The naive baseline version of rotate 
 */
char naive_rotate_descr[] = "naive_rotate: Naive baseline implementation";
void naive_rotate(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(dim-1-j, i, dim)] = src[RIDX(i, j, dim)];
}

/* 
 * rotate - Your current working version of rotate
 * IMPORTANT: This is the version you will be graded on
 */
char rotate_descr[] = "rotate: Current working version----loop unrolling for 16 column";
void rotate(int dim, pixel *src, pixel *dst) 
{
    //loop unrolling for 16 column
    pixel tmp[16];
    for(int i = 0; i < dim; i++)
    {
        for(int j = 0;j < dim; j += 16)
        {
            //store src into tmp array
            tmp[0] = src[RIDX(i, j, dim)];
            tmp[1] = src[RIDX(i, j+1, dim)];
            tmp[2] = src[RIDX(i, j+2, dim)];
            tmp[3] = src[RIDX(i, j+3, dim)];
            tmp[4] = src[RIDX(i, j+4, dim)];
            tmp[5] = src[RIDX(i, j+5, dim)];
            tmp[6] = src[RIDX(i, j+6, dim)];
            tmp[7] = src[RIDX(i, j+7, dim)];
            tmp[8] = src[RIDX(i, j+8, dim)];
            tmp[9] = src[RIDX(i, j+9, dim)];
            tmp[10] = src[RIDX(i, j+10, dim)];
            tmp[11] = src[RIDX(i, j+11, dim)];
            tmp[12] = src[RIDX(i, j+12, dim)];
            tmp[13] = src[RIDX(i, j+13, dim)];
            tmp[14] = src[RIDX(i, j+14, dim)];
            tmp[15] = src[RIDX(i, j+15, dim)];

            //variable dst array from tmp
            dst[RIDX(dim-1-j, i, dim)] = tmp[0]; 
            dst[RIDX(dim-2-j, i, dim)] = tmp[1]; 
            dst[RIDX(dim-3-j, i, dim)] = tmp[2]; 
            dst[RIDX(dim-4-j, i, dim)] = tmp[3]; 
            dst[RIDX(dim-5-j, i, dim)] = tmp[4]; 
            dst[RIDX(dim-6-j, i, dim)] = tmp[5];
            dst[RIDX(dim-7-j, i, dim)] = tmp[6]; 
            dst[RIDX(dim-8-j, i, dim)] = tmp[7]; 
            dst[RIDX(dim-9-j, i, dim)] = tmp[8]; 
            dst[RIDX(dim-10-j, i, dim)] = tmp[9]; 
            dst[RIDX(dim-11-j, i, dim)] = tmp[10];
            dst[RIDX(dim-12-j, i, dim)] = tmp[11];
            dst[RIDX(dim-13-j, i, dim)] = tmp[12];
            dst[RIDX(dim-14-j, i, dim)] = tmp[13];
            dst[RIDX(dim-15-j, i, dim)] = tmp[14];
            dst[RIDX(dim-16-j, i, dim)] = tmp[15];
        }
    }
}

char rotate_descr1[] = "rotate: Current working version1----loop unrolling for 32 column";
void rotate1(int dim, pixel *src, pixel *dst) 
{
    //loop unrolling for 32 column
    pixel tmp[32];
    for(int i = 0; i < dim; i++)
    {
        for(int j = 0;j < dim; j += 32)
        {
            //store src into tmp array
            tmp[0] = src[RIDX(i, j, dim)];
            tmp[1] = src[RIDX(i, j+1, dim)];
            tmp[2] = src[RIDX(i, j+2, dim)];
            tmp[3] = src[RIDX(i, j+3, dim)];
            tmp[4] = src[RIDX(i, j+4, dim)];
            tmp[5] = src[RIDX(i, j+5, dim)];
            tmp[6] = src[RIDX(i, j+6, dim)];
            tmp[7] = src[RIDX(i, j+7, dim)];
            tmp[8] = src[RIDX(i, j+8, dim)];
            tmp[9] = src[RIDX(i, j+9, dim)];
            tmp[10] = src[RIDX(i, j+10, dim)];
            tmp[11] = src[RIDX(i, j+11, dim)];
            tmp[12] = src[RIDX(i, j+12, dim)];
            tmp[13] = src[RIDX(i, j+13, dim)];
            tmp[14] = src[RIDX(i, j+14, dim)];
            tmp[15] = src[RIDX(i, j+15, dim)];
            tmp[16] = src[RIDX(i, j+16, dim)];
            tmp[17] = src[RIDX(i, j+17, dim)];
            tmp[18] = src[RIDX(i, j+18, dim)];
            tmp[19] = src[RIDX(i, j+19, dim)];
            tmp[20] = src[RIDX(i, j+20, dim)];
            tmp[21] = src[RIDX(i, j+21, dim)];
            tmp[22] = src[RIDX(i, j+22, dim)];
            tmp[23] = src[RIDX(i, j+23, dim)];
            tmp[24] = src[RIDX(i, j+24, dim)];
            tmp[25] = src[RIDX(i, j+25, dim)];
            tmp[26] = src[RIDX(i, j+26, dim)];
            tmp[27] = src[RIDX(i, j+27, dim)];
            tmp[28] = src[RIDX(i, j+28, dim)];
            tmp[29] = src[RIDX(i, j+29, dim)];
            tmp[30] = src[RIDX(i, j+30, dim)];
            tmp[31] = src[RIDX(i, j+31, dim)];

            //variable dst array from tmp
            dst[RIDX(dim-1-j, i, dim)] = tmp[0]; 
            dst[RIDX(dim-2-j, i, dim)] = tmp[1]; 
            dst[RIDX(dim-3-j, i, dim)] = tmp[2]; 
            dst[RIDX(dim-4-j, i, dim)] = tmp[3]; 
            dst[RIDX(dim-5-j, i, dim)] = tmp[4]; 
            dst[RIDX(dim-6-j, i, dim)] = tmp[5];
            dst[RIDX(dim-7-j, i, dim)] = tmp[6]; 
            dst[RIDX(dim-8-j, i, dim)] = tmp[7]; 
            dst[RIDX(dim-9-j, i, dim)] = tmp[8]; 
            dst[RIDX(dim-10-j, i, dim)] = tmp[9]; 
            dst[RIDX(dim-11-j, i, dim)] = tmp[10];
            dst[RIDX(dim-12-j, i, dim)] = tmp[11];
            dst[RIDX(dim-13-j, i, dim)] = tmp[12];
            dst[RIDX(dim-14-j, i, dim)] = tmp[13];
            dst[RIDX(dim-15-j, i, dim)] = tmp[14];
            dst[RIDX(dim-16-j, i, dim)] = tmp[15];
            dst[RIDX(dim-17-j, i, dim)] = tmp[16]; 
            dst[RIDX(dim-18-j, i, dim)] = tmp[17]; 
            dst[RIDX(dim-19-j, i, dim)] = tmp[18]; 
            dst[RIDX(dim-20-j, i, dim)] = tmp[19]; 
            dst[RIDX(dim-21-j, i, dim)] = tmp[20]; 
            dst[RIDX(dim-22-j, i, dim)] = tmp[21];
            dst[RIDX(dim-23-j, i, dim)] = tmp[22]; 
            dst[RIDX(dim-24-j, i, dim)] = tmp[23]; 
            dst[RIDX(dim-25-j, i, dim)] = tmp[24]; 
            dst[RIDX(dim-26-j, i, dim)] = tmp[25]; 
            dst[RIDX(dim-27-j, i, dim)] = tmp[26];
            dst[RIDX(dim-28-j, i, dim)] = tmp[27];
            dst[RIDX(dim-29-j, i, dim)] = tmp[28];
            dst[RIDX(dim-30-j, i, dim)] = tmp[29];
            dst[RIDX(dim-31-j, i, dim)] = tmp[30];
            dst[RIDX(dim-32-j, i, dim)] = tmp[31];
        }
    }
}

char rotate_descr2[] = "rotate: Current working version2----loop unrolling for column and loop hoisting for macro";
void rotate2(int dim, pixel *src, pixel *dst) 
{
    //loop unrolling for 16 column
    pixel tmp[16];
    //loop hoisting
    int r;
    for(int i = 0; i < dim; i++)
    {
        for(int j = 0;j < dim; j += 16)
        {
            //multiply macro
            r = RIDX(i, j, dim);//r = i * dim + j
            //store src into tmp array
            tmp[0] = src[r];
            tmp[1] = src[r+1];
            tmp[2] = src[r+2];
            tmp[3] = src[r+3];
            tmp[4] = src[r+4];
            tmp[5] = src[r+5];
            tmp[6] = src[r+6];
            tmp[7] = src[r+7];
            tmp[8] = src[r+8];
            tmp[9] = src[r+9];
            tmp[10] = src[r+10];
            tmp[11] = src[r+11];
            tmp[12] = src[r+12];
            tmp[13] = src[r+13];
            tmp[14] = src[r+14];
            tmp[15] = src[r+15];

            //multiply macro
            r = RIDX(dim-1-j, i, dim);//r = (dim-1-j) * dim + i
            //variable dst array from tmp
            dst[r] = tmp[0]; 
            dst[r-1*dim] = tmp[1]; 
            dst[r-2*dim] = tmp[2]; 
            dst[r-3*dim] = tmp[3]; 
            dst[r-4*dim] = tmp[4]; 
            dst[r-5*dim] = tmp[5];
            dst[r-6*dim] = tmp[6]; 
            dst[r-7*dim] = tmp[7]; 
            dst[r-8*dim] = tmp[8]; 
            dst[r-9*dim] = tmp[9]; 
            dst[r-10*dim] = tmp[10];
            dst[r-11*dim] = tmp[11];
            dst[r-12*dim] = tmp[12];
            dst[r-13*dim] = tmp[13];
            dst[r-14*dim] = tmp[14];
            dst[r-15*dim] = tmp[15];
        }
    }
}
char rotate_descr3[] = "rotate: Current working version----loop unrolling for 16 column and replace address from pointer";
void rotate3(int dim, pixel *src, pixel *dst) 
{
    //loop unrolling for 16 column
    pixel tmp[16];
    //two address to store the beginning address of two pixel array
    //pixel *src_addr = src; to store source address is useless
    pixel *dst_addr = dst;
    for(int i = 0; i < dim; i++)
    {
        for(int j = 0;j < dim; j += 16)
        {
            //store src into tmp array

            //use pointer to replace address calculation
            tmp[0] = *src++;
            tmp[1] = *src++;
            tmp[2] = *src++;
            tmp[3] = *src++;
            tmp[4] = *src++;
            tmp[5] = *src++;
            tmp[6] = *src++;
            tmp[7] = *src++;
            tmp[8] = *src++;
            tmp[9] = *src++;
            tmp[10] = *src++;
            tmp[11] = *src++;
            tmp[12] = *src++;
            tmp[13] = *src++;
            tmp[14] = *src++;
            tmp[15] = *src++;

            //variable dst array from tmp

            //relocate dst address
            dst = dst_addr + RIDX(dim-1-j, i, dim);

            *dst = tmp[0]; 
            dst -= dim;
            *dst = tmp[1]; 
            dst -= dim;
            *dst = tmp[2]; 
            dst -= dim;
            *dst = tmp[3]; 
            dst -= dim;
            *dst = tmp[4]; 
            dst -= dim;
            *dst = tmp[5];
            dst -= dim;
            *dst = tmp[6]; 
            dst -= dim;
            *dst = tmp[7]; 
            dst -= dim;
            *dst = tmp[8]; 
            dst -= dim;
            *dst = tmp[9]; 
            dst -= dim;
            *dst = tmp[10];
            dst -= dim;
            *dst = tmp[11];
            dst -= dim;
            *dst = tmp[12];
            dst -= dim;
            *dst = tmp[13];
            dst -= dim;
            *dst = tmp[14];
            dst -= dim;
            *dst = tmp[15];
        }
    }
}
char rotate_descr4[] = "rotate: Current working version----loop unrolling for 32 column and replace address from pointer";
void rotate4(int dim, pixel *src, pixel *dst) 
{
    //two address to store the beginning address of two pixel array
    //pixel *src_addr = src; to store source address is useless
    pixel *dst_addr = dst;
    for(int i = 0; i < dim; i++)
    {
        for(int j = 0;j < dim; j += 32)
        {
            //relocate dst address
            dst = dst_addr + RIDX(dim-1-j, i, dim);
            //use pointer to replace address calculation
            //1
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;

            //2
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;

            //3
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;

            //4
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;

            //5
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;

            //6
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;

            //7
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;

            //8
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
            *dst= *src++;
            dst -= dim;
        }
    }
}
char rotate_descr5[] = "rotate: Current working version----expand macro and execute column by column also loop hoisting";
void rotate5(int dim, pixel *src, pixel *dst) 
{
    //define two varibles to store address of src and dst
    int src_addr;
    int dst_addr; 
    for(int i = 0;i < dim; i += 32)
    {
        for(int j = 0;j < dim; j++)
        {
            //exchange elements by column
            src_addr = i*dim+j;
            dst_addr = (dim-1-j)*dim+i;
            for(int k = 0;k < 32; k++)
            {
                dst[dst_addr+k] = src[src_addr+dim*k];
            }
        }
    }
}
/*********************************************************************
 * register_rotate_functions - Register all of your different versions
 *     of the rotate kernel with the driver by calling the
 *     add_rotate_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_rotate_functions() 
{
    add_rotate_function(&naive_rotate, naive_rotate_descr);   
    add_rotate_function(&rotate, rotate_descr);   
    add_rotate_function(&rotate1, rotate_descr1);   
    add_rotate_function(&rotate2, rotate_descr2);   
    add_rotate_function(&rotate3, rotate_descr3);   
    add_rotate_function(&rotate4, rotate_descr4);   
    add_rotate_function(&rotate5, rotate_descr5);     
    /* ... Register additional test functions here */
}


/***************
 * SMOOTH KERNEL
 **************/

/***************************************************************
 * Various typedefs and helper functions for the smooth function
 * You may modify these any way you like.
 **************************************************************/

/* A struct used to compute averaged pixel value */
typedef struct {
    int red;
    int green;
    int blue;
    int num;
} pixel_sum;

/* Compute min and max of two integers, respectively */
static int min(int a, int b) { return (a < b ? a : b); }
static int max(int a, int b) { return (a > b ? a : b); }

/* 
 * initialize_pixel_sum - Initializes all fields of sum to 0 
 */
static void initialize_pixel_sum(pixel_sum *sum) 
{
    sum->red = sum->green = sum->blue = 0;
    sum->num = 0;
    return;
}

/* 
 * accumulate_sum - Accumulates field values of p in corresponding 
 * fields of sum 
 */
static void accumulate_sum(pixel_sum *sum, pixel p) 
{
    sum->red += (int) p.red;
    sum->green += (int) p.green;
    sum->blue += (int) p.blue;
    sum->num++;
    return;
}

/* 
 * assign_sum_to_pixel - Computes averaged pixel value in current_pixel 
 */
static void assign_sum_to_pixel(pixel *current_pixel, pixel_sum sum) 
{
    current_pixel->red = (unsigned short) (sum.red/sum.num);
    current_pixel->green = (unsigned short) (sum.green/sum.num);
    current_pixel->blue = (unsigned short) (sum.blue/sum.num);
    return;
}

/* 
 * avg - Returns averaged pixel value at (i,j) 
 */
static pixel avg(int dim, int i, int j, pixel *src) 
{
    int ii, jj;
    pixel_sum sum;
    pixel current_pixel;

    initialize_pixel_sum(&sum);
    for(ii = max(i-1, 0); ii <= min(i+1, dim-1); ii++) 
	for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	    accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);

    assign_sum_to_pixel(&current_pixel, sum);
    return current_pixel;
}

/******************************************************
 * Your different versions of the smooth kernel go here
 ******************************************************/

/*
 * naive_smooth - The naive baseline version of smooth 
 */
char naive_smooth_descr[] = "naive_smooth: Naive baseline implementation";
void naive_smooth(int dim, pixel *src, pixel *dst) 
{
    int i, j;

    for (i = 0; i < dim; i++)
	for (j = 0; j < dim; j++)
	    dst[RIDX(i, j, dim)] = avg(dim, i, j, src);
}

/*
 * smooth - Your current working version of smooth. 
 * IMPORTANT: This is the version you will be graded on
 */
char smooth_descr[] = "smooth: Current working version----calculate by column";
void smooth(int dim, pixel *src, pixel *dst) 
{
    for(int i = 0;i < dim; i += 32)
    {
        for(int j = 0;j < dim; j++)
        {
            for(int k = 0;k < 32; k++)
            {
	            dst[RIDX(i+k, j, dim)] = avg(dim, i+k, j, src);
            }
        }
    }
}

char smooth_descr1[] = "smooth: Current working version----Inline function optimization also calculate by column";
void smooth1(int dim, pixel *src, pixel *dst) 
{
    for(int i = 0;i < dim; i += 32)
    {
        for(int j = 0;j < dim; j++)
        {
            for(int k = 0;k < 32; k++)
            {
                //Inline function
	            //dst[RIDX(i+k, j, dim)] = avg(dim, i+k, j, src);
                pixel_sum sum = {0,0,0,0};
                int ii, jj;
                for(ii = max(i+k-1, 0); ii <= min(i+k+1, dim-1); ii++) 
                {
	                for(jj = max(j-1, 0); jj <= min(j+1, dim-1); jj++) 
	                {
                        //accumulate_sum(&sum, src[RIDX(ii, jj, dim)]);
                        sum.red += (int) src[RIDX(ii, jj, dim)].red;
                        sum.green += (int) src[RIDX(ii, jj, dim)].green;
                        sum.blue += (int) src[RIDX(ii, jj, dim)].blue;
                        sum.num++;
                    }    
                }
                dst[RIDX(i+k, j, dim)].red = (unsigned short) (sum.red/sum.num);
                dst[RIDX(i+k, j, dim)].green = (unsigned short) (sum.green/sum.num);
                dst[RIDX(i+k, j, dim)].blue = (unsigned short) (sum.blue/sum.num);
            }
        }
    }
}

/********************************************************************* 
 * register_smooth_functions - Register all of your different versions
 *     of the smooth kernel with the driver by calling the
 *     add_smooth_function() for each test function.  When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.  
 *********************************************************************/

void register_smooth_functions() {
    add_smooth_function(&naive_smooth, naive_smooth_descr);
    add_smooth_function(&smooth, smooth_descr);
    add_smooth_function(&smooth1, smooth_descr1);
    /* ... Register additional test functions here */
}

