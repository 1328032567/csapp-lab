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
/* size -> header + payload + footer, prev_alloc -> previous block whether allocated, alloc -> block whether allocated*/
#define PACK(size, prev_alloc, alloc)   ((size) | (prev_alloc)<<1 | (alloc))

/* Read and write a word at address p */
#define GET(p)          (*(unsigned int *)(p))
#define PUT(p, val)     (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)         (GET(p) & ~0x7)     /* Sum of header, footer and payload */
#define GET_ALLOC(p)        (GET(p) & 0x1)
#define GET_PREV_ALLOC(p)   (GET(p) & 0x2)

/* Read the flag bit, and set the value from p */
#define SET_ALLOC(p)            (GET(p) &= 0x1)
#define SET_FREE(p)             (GET(p) &= ~0x1)
#define SET_PREV_ALLOC(p)       (GET(p) &= 0x2)
#define SET_PREV_FREE(p)        (GET(p) &= ~0x2)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE(((char *)(bp)))) 
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* Given block ptr bp, compute address of next and previous nodes in the list*/
#define GET_NEXT_NODE(bp)   ((char *)(mem_heap_lo() + *(unsigned int *)((bp) + WSIZE)))
#define GET_PREV_NODE(bp)   ((char *)(mem_heap_lo() + *(unsigned int *)(bp)))

/* Given block ptr bp, set previous pointer and next pointer address */
#define SET_NEXT_POINTER(bp, addr)     (*(unsigned int *)((char *)(bp) + WSIZE) = (unsigned int)(addr))
#define SET_PREV_POINTER(bp, addr)     (*(unsigned int *)(bp) = (unsigned int)(addr))

/* Define the number of the free_list */
#define FREE_LIST_NUM 15

/* Define gobal free list to store head pointer */
static char **free_list;
/* Additional Functions*/
static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);

static int get_index(size_t size);

static void delete_node(void* bp);
static void insert_node(void* bp);

/* Define a global variable to point to the end of prologue block */
static void *heap_listp;

/*
 * Description of the free_list:
 *      head node: previous_pointer -> NULL, next_pointer -> normal node
 *      tail node: previous_pointer -> normal node, next_pointer -> NULL
 *      free_list[index]: point to the free head node's address.
        image:                   /--------------\               /--------------\               /---------------\
        free_list[index]        V                \             V                \             V                 \
        address     --->    [head_node]    |--->  \    [normal_node1]  |--->     \     [normal_node2]    |--->   \     [tail_node]
               NULL <---    previous      |        \-- previous       |           \--  previous         |         \--  previous
                            next       --|             next        --|                 next          --|               next     ---> NULL
 */
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    /* Initialize free_list position */
    free_list = mem_heap_lo();
    for(int i = 0; i < FREE_LIST_NUM; i++){
        if((heap_listp = mem_sbrk(WSIZE)) == (void *)(-1))
            return -1;
        //different free_list[i] = mem_heap_lo();
        free_list[i] = NULL;
    }
    /* Create the prologue header, footer and epilogue header at heap */
    if((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);                             /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 0, 1));    /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 0, 1));    /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 0, 1));        /* Epilogue header */
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
   size_t prev_alloc = GET_PREV_ALLOC(HDRP(bp));
   PUT(HDRP(bp), PACK(size, prev_alloc, 0));    /* New free block header */
   PUT(FTRP(bp), PACK(size, prev_alloc, 0));    /* New free block footer */
   PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 0, 1));    /* New epilogue header */

   /* Coalesce if the previous block was free */
   return coalesce(bp);
}

/*
 * get_index - get free_list index by free block size.
 */
static int get_index(size_t size)
{
    /* Judge the range of size, and return list index */
    if(size > 0 && size <= 24)
        return 0;
    else if(size <= 32)
        return 1;
    else if(size <= 64)
        return 2;
    else if(size <= 80)
        return 3;
    else if(size <= 120)
        return 4;
    else if(size <= 240)
        return 5;
    else if(size <= 480)
        return 6;
    else if(size <= 960)
        return 7;
    else if(size <= 1920)
        return 8;
    else if(size <= 3840)
        return 9;
    else if(size <= 7680)
        return 10;
    else if(size <= 15360)
        return 11;
    else if(size <= 30720)
        return 12;
    else if(size <= 61440)
        return 13;
    else 
        return 14;
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
    /* First-fit search */
    void *bp;
    for(bp = heap_listp; GET_SIZE(HDRP(bp)) > 0; bp = NEXT_BLKP(bp)){
        if(!GET_ALLOC(HDRP(bp)) && (asize <= GET_SIZE(HDRP(bp)))){
            return bp;
        }
    }
    return NULL; /* No fit */
    
    // /* Best-fit search */
    // void *bp = heap_listp;
    // void *best_bp;
    // int alloc;
    // size_t size;
    // size_t rest;
    // size_t min_rest = 0xffffffff;
    // int flag = 0;

    // while((size = GET_SIZE(HDRP(bp))) > 0){ /* Not the epilogue block */
    //     if((alloc = GET_ALLOC(HDRP(bp))) == 0 && asize <= size){ /* Not allocated */
    //         rest = size - asize;
    //         flag = 1;
    //         if(rest == 0){
    //             return bp;
    //         }
    //         else if(rest < min_rest){ /* Update rest and bp pointer */
    //             min_rest = rest;
    //             best_bp = bp;
    //         }
    //     }
    //     bp = NEXT_BLKP(bp);
    // }
    // if(flag == 1){ /* Best match*/
    //     return best_bp;
    // }
    // return NULL;/* No fit block */
}

/*
 * place - Place the request block at the beginning of the free block.
 */
static void place(void *bp, size_t asize)
{
    size_t size = GET_SIZE(HDRP(bp));
    size_t rest = size - asize;
    if(rest < (2*DSIZE)){ /* Not split*/
       PUT(HDRP(bp), PACK(size, 1));
       PUT(FTRP(bp), PACK(size, 1));
    }
    else{ /* Split */
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(rest, 0));
        PUT(FTRP(bp), PACK(rest, 0));
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
        SET_PREV_ALLOC(HDRP(NEXT_BLKP(bp)));
    }

    else if (prev_alloc && !next_alloc) {       /* Case 2 */
        delete_node(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 1, 0));
        PUT(FTRP(bp), PACK(size, 1, 0));
    }

    else if (!prev_alloc && next_alloc) {       /* Case 3 */
        delete_node(PREV_NODE(bp));
        SET_PREV_FREE(HDRP(NEXT_NODE(bp)));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        size_t prev_prev_alloc = GET_PREV_ALLOC(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, prev_prev_alloc, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, prev_prev_alloc, 0));
        bp = PREV_BLKP(bp); 
    }
    else if (!prev_alloc && !next_alloc) {      /* Case 4 */
        delete_node(PREV_NODE(bp));
        delete_node(NEXT_NODE(bp));
        SET_PREV_FREE(HDRP(NEXT_NODE(NEXT_NODE(bp))));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
        size_t prev_prev_alloc = GET_PREV_ALLOC(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, prev_prev_alloc, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, prev_prev_alloc, 0));
        bp = PREV_BLKP(bp);
    }
    insert_node(bp);
    return bp;
}

/*
 * insert_node - insert the node to the free list.
 */
static void insert_node(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));
    int index = get_index(size);
    char *newptr = bp;
    char *oldptr = free_list[index];
    free_list[index] = newptr;  /* Insert new node to the head of the list. */

    if(oldptr == NULL){ /* free_list[index] point to tail */
        SET_PREV_POINTER(newptr, NULL);
        SET_NEXT_POINTER(newptr, NULL);
    }
    else{   /* Insert new node to the list. */
        SET_PREV_POINTER(newptr, NULL);
        SET_NEXT_POINTER(newptr, oldptr);
        SET_PREV_POINTER(oldptr, newptr);
    }
}

/*
 * delete_node - delete the free node from the list.
 */
static void delete_node(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));
    int index = get_index(size);
    char *prevptr = GET_PREV_NODE(bp);
    char *nextptr = GET_NEXT_NODE(bp);

    if(prevptr == NULL){    /* Delete head node */
        free_list[index] = nextptr;
        SET_PREV_POINTER(nextptr, NULL);
    }
    else if(nextptr == NULL){   /* Delete tail node */
        SET_NEXT_POINTER(prevptr, NULL);
    }
    else{   /* Delete normal node */
        SET_NEXT_POINTER(prevptr, nextptr);
        SET_PREV_POINTER(nextptr, prevptr);
    }
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
    copySize = GET_SIZE(HDRP(ptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}














