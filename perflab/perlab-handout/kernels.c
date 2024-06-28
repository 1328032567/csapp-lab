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
char rotate_descr[] = "rotate: Current working version";
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

char rotate_descr1[] = "rotate: Current working version1";
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
char smooth_descr[] = "smooth: Current working version";
void smooth(int dim, pixel *src, pixel *dst) 
{
    naive_smooth(dim, src, dst);
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
    /* ... Register additional test functions here */
}

