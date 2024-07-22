/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "MallocTeam",
    /* First member's full name */
    "Xuan",
    /* First member's email address */
    "xuan@edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)

#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

/* Basic constants and macros*/
#define WSIZE           4       /* Word and header/footer size (bytes) */
#define DSIZE           8       /* Double word size (bytes) */
#define CHUNKSIZE       (1<<12) /* Extend heap by this amount (bytes) */

/* Max number*/
#define MAX(x, y)       ((x) > (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
/* size -> header + payload + footer, alloc -> block whether allocated*/
#define PACK(size, alloc)   ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)          (*(unsigned int *)(p))
#define PUT(p, val)     (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (GET(p) & ~0x7)     /* Sum of header, footer and payload */
#define GET_ALLOC(p)    (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE))) 
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* Additional Functions*/
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);

/* Define a global variable to point to the end of prologue block */
static void *heap_listp;
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);                             /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 1));    /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 1));    /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 1));        /* Epilogue header */
    heap_listp += (2*WSIZE);

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if(extend_heap(CHUNKSIZE/WSIZE) == NULL)
        return -1;
    return 0;
}

/*
 * extend_heap - call mem_sbrk function to allocate extra heap space to expand.
 */
static void *extend_heap(size_t words)
{
   char *bp;
   size_t size;

   /* Allocate an even number of words to maintain alignment */
   size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
   if((long)(bp = mem_sbrk(size)) == -1)
       return NULL;

   /* Initialize free block header/footer and the epilogue header */
   PUT(HDRP(bp), PACK(size, 0));            /* Free block header */
   PUT(FTRP(bp), PACK(size, 0));            /* Free block footer */
   PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));    /* New epilogue header */

   /* Coalesce if the previous block was free */
   return coalesce(bp);
}
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size_t asize;       /* Adjusted block size */
    size_t extendsize;  /* Amount to extend heap if no fit */
    char *bp;

    /* Ignore spurious requests */
    if(size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if(size <= DSIZE)
        asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

    /* Search the free list for a fit */
    if((bp = find_fit(asize)) != NULL) {
        place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * find_fit - Find a fitable block to allocate.
 */
static void *find_fit(size_t asize)
{
    /* Initialize bp pointer */
    char *bp = heap_listp;

    size_t size;
    int alloc;
    while(!((size = GET_SIZE(HDRP(bp))) == 0 && (alloc = GET_ALLOC(HDRP(bp))) == 1))/* Not the epilogue block */
    {
        if(size >= asize && alloc == 0) /* Find the fit block */
            return bp;
        else /* Dont fit, point to next block */
            bp = NEXT_BLKP(bp);
    }
    /* Allocate failed */
    return NULL;
}

/*
 * place - Place the request block at the beginning of the free block.
 */
static void place(void *bp, size_t asize)
{
    size_t size = GET_SIZE(HDRP(bp));
    size_t rest = size - asize;
    if(rest < 16){ /* Not split*/
       PUT(HDRP(bp), PACK(size, 1));
       PUT(FTRP(bp), PACK(size, 1));
    }
    else{ /* Split */
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(rest, 0));

        PUT((char *)bp + asize, PACK(asize, 1));
        PUT((char *)bp + asize + WSIZE, PACK(rest, 0));
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    coalesce(bp);
}

/*
 * coalesce - coalesce adjacent free blocks.
 */
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {             /* Case 1 */
        return bp;
    }

    else if (prev_alloc && !next_alloc) {       /* Case 2 */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
    }

    else if (!prev_alloc && next_alloc) {       /* Case 3 */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp); 
    }
    else if (!prev_alloc && !next_alloc) {      /* Case 4 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
    }
    return bp;
}
/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    void *oldptr = ptr;
    void *newptr;
    size_t copySize;
    
    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;
    copySize = *(size_t *)((char *)oldptr - SIZE_T_SIZE);
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














