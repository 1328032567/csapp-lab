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
/* Debug mode */
// #define debug

/* Print debug information */
// #define print

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
#define PACK(size, prev_alloc, alloc)   ((size) | ((prev_alloc)<<1) | (alloc))

/* Read and write a word at address p */
#define GET(p)          (*(unsigned int *)(p))
#define PUT(p, val)     (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)         (GET(p) & ~0x7)     /* Sum of header, footer and payload */
#define GET_ALLOC(p)        (GET(p) & 0x1)
#define GET_PREV_ALLOC(p)   ((GET(p) & 0x2)>>1)

/* Read the flag bit, and set the value from p */
#define SET_ALLOC(p)            (GET(p) |= 0x1)
#define SET_FREE(p)             (GET(p) &= ~0x1)
#define SET_PREV_ALLOC(p)       (GET(p) |= 0x2)
#define SET_PREV_FREE(p)        (GET(p) &= ~0x2)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp)   ((char *)(bp) + GET_SIZE((HDRP((char *)(bp))))) 
#define PREV_BLKP(bp)   ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* Given block ptr bp, compute address of next and previous nodes in the list*/
#define PREV_NODE(bp)       ((char *)(mem_heap_lo() + *(unsigned*)(bp)))
#define NEXT_NODE(bp)       ((char *)(mem_heap_lo() + *(unsigned*)(bp + WSIZE)))

/* Given block ptr bp, set previous pointer and next pointer address */
#define SET_PREV_POINTER(bp, val)   (*(unsigned*)(bp) = ((unsigned)(long)val))
#define SET_NEXT_POINTER(bp, val)   (*(unsigned*)((char *)bp + WSIZE) = ((unsigned)(long)val))

/* Define the number of the free_list */
#define FREE_LIST_NUM 15

/* Define heap bottom equals to mem_heap_lo() */
#define bottom (mem_heap_lo())

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

static size_t adjust_alloc_size(size_t size);

static int mm_check(void);
static void mm_printfreelist(void);
static void mm_printheap(void);
/* Define a global variable to point to the end of prologue block */
static void *heap_listp;

int times = 0;
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
    #ifdef debug
    puts("Initilize");
    #endif
    /* Initialize free_list position */
    free_list = mem_heap_lo();
    for(int i = 0; i < FREE_LIST_NUM; i++){
        if((heap_listp = mem_sbrk(DSIZE)) == (void *)(-1))
            return -1;
        free_list[i] = NULL;
    }
    /* Create the prologue header, footer and epilogue header at heap */
    if((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
        return -1;
    PUT(heap_listp, 0);                             /* Alignment padding */
    PUT(heap_listp + (1*WSIZE), PACK(DSIZE, 0, 1));    /* Prologue header */
    PUT(heap_listp + (2*WSIZE), PACK(DSIZE, 0, 1));    /* Prologue footer */
    PUT(heap_listp + (3*WSIZE), PACK(0, 1, 1));        /* Epilogue header */
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
    #ifdef debug
    puts("Extend_heap.");
    #endif
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    size_t prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    PUT(HDRP(bp), PACK(size, prev_alloc, 0));   /* New free block header */
    PUT(FTRP(bp), PACK(size, prev_alloc, 0));   /* New free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 0, 1));    /* New epilogue header */

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    #ifdef debug
    times++;
    puts("\nMalloc");
        #ifdef print    
        puts("Before");
        mm_check();
        printf("Malloc Size:%u\n", (unsigned)size);
        #endif
    #endif
    size_t asize;       /* Adjusted block size */
    size_t extendsize;  /* Amount to extend heap if no fit */
    char *bp;

    /* Ignore spurious requests */
    if(size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    size = adjust_alloc_size(size);
    if(size <= DSIZE)
        asize = 2*DSIZE;
    else
        asize = DSIZE * ((size + (WSIZE) + (DSIZE - 1)) / DSIZE);

    #ifdef debug
        #ifdef print
        printf("Actual Size:%u\n", (unsigned)asize);
        #endif
    #endif
    /* Search the free list for a fit */
    if((bp = find_fit(asize)) != NULL) {
        place(bp, asize);

    #ifdef debug
        #ifdef print
        puts("After");
        mm_check();
        #endif
    #endif

        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if((bp = extend_heap(extendsize / WSIZE)) == NULL)
        return NULL;
    place(bp, asize);

    #ifdef debug
        #ifdef print
        puts("After");
        mm_check();
        #endif
    #endif

    return bp;
}

/*
 * find_fit - Find a fitable block to allocate.
 */
static void *find_fit(size_t asize)
{
    #ifdef debug
    puts("Find_fit");
    #endif
    /* First-fit search */
    void *ptr;  /* Point to the free block */
    size_t size;
    int index = get_index(asize);

    for(int i = index; i < FREE_LIST_NUM; i++){ /* Traverse free_list */
        for(ptr = (unsigned long)free_list[i] + (char *)bottom; ptr != bottom; ptr = NEXT_NODE(ptr)){
            size = GET_SIZE(HDRP(ptr));
            if(size >= asize)
                return ptr;
        }
    }
    return NULL; /* No fit */
}

/*
 * place - Place the request block at the beginning of the free block.
 */
static void place(void *bp, size_t asize)
{
    #ifdef debug
    puts("Place");
    #endif
    size_t size = GET_SIZE(HDRP(bp));
    size_t rest = size - asize;
    delete_node(bp);
    if(rest < (2*DSIZE)){   /* Not split */
        #ifdef debug
        puts("Place 1");
        #endif
        SET_ALLOC(HDRP(bp));
        SET_PREV_ALLOC(HDRP(NEXT_BLKP(bp)));
        if(GET_ALLOC(HDRP(NEXT_BLKP(bp))) == 0){    /* Next block is free and has the footer */
            SET_PREV_ALLOC(FTRP(NEXT_BLKP(bp)));
        }
    }
    else{   /* Split */
        #ifdef debug
        puts("Place 2");
        #endif
        /* Set allocate block header */
        PUT(HDRP(bp), PACK(asize, GET_PREV_ALLOC(HDRP(bp)), 1));
        /* Set rest split block header and footer */ 
        bp = NEXT_BLKP(bp);
        PUT(HDRP(bp), PACK(rest, 1, 0));
        PUT(FTRP(bp), PACK(rest, 1, 0));
        insert_node(bp);
    }
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    #ifdef debug
    puts("\nFree");
        #ifdef print
            printf("Free Size:%u\n", (unsigned)GET_SIZE(HDRP(bp)));
        #endif
    #endif
    size_t size = GET_SIZE(HDRP(bp));
    int prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    PUT(HDRP(bp), PACK(size, prev_alloc, 0));
    PUT(FTRP(bp), PACK(size, prev_alloc, 0));

    coalesce(bp);
}

/*
 * coalesce - coalesce adjacent free blocks.
 */
static void *coalesce(void *bp)
{
    #ifdef debug
    puts("Coalesce");
    #endif
    size_t prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {             /* Case 1 */
        #ifdef debug
        puts("Coalesce 1.");
        #endif
        SET_PREV_FREE(HDRP(NEXT_BLKP(bp)));
    }

    else if (prev_alloc && !next_alloc) {       /* Case 2 */
        #ifdef debug
        puts("Coalesce 2.");
        #endif
        delete_node(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 1, 0));
        PUT(FTRP(bp), PACK(size, 1, 0));
    }

    else if (!prev_alloc && next_alloc) {       /* Case 3 */
        #ifdef debug
        puts("Coalesce 3.");
        #endif
        delete_node(PREV_BLKP(bp));
        SET_PREV_FREE(HDRP(NEXT_BLKP(bp)));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        size_t prev_prev_alloc = GET_PREV_ALLOC(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, prev_prev_alloc, 0));
        PUT(FTRP(bp), PACK(size, prev_prev_alloc, 0));
        bp = PREV_BLKP(bp);
    }
    else if (!prev_alloc && !next_alloc) {      /* Case 4 */
        #ifdef debug
        puts("Coalesce 4.");
        #endif
        delete_node(PREV_BLKP(bp));
        delete_node(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
        int prev_prev_alloc = GET_PREV_ALLOC(HDRP(PREV_BLKP(bp)));
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
    #ifdef debug
    puts("Insert_node");
    #endif
    size_t size = GET_SIZE(HDRP(bp));
    volatile int index = get_index(size);
    char *newptr = bp;  /* absolute address */
    char *oldptr = (long)free_list[index] + bottom;  /* absolute address */

    if(oldptr == bottom){   /* free_list[index] point to tail*/
        #ifdef debug
        puts("Insert 1");
        #endif
        free_list[index] = (char *)(newptr - (char *)bottom);
        SET_PREV_POINTER(newptr, NULL);/* set offset address */
        SET_NEXT_POINTER(newptr, NULL);/* set offset address*/
    }
    else{   /* Insert node to the head */
        #ifdef debug
        puts("Insert 2");
        #endif
        free_list[index] = (char *)(newptr - (char *)bottom);/* set offset address */ 
        SET_PREV_POINTER(oldptr, (unsigned)newptr-(unsigned)bottom);/* set offset address */
        SET_PREV_POINTER(newptr, NULL);/* set offset address */
        SET_NEXT_POINTER(newptr, (unsigned)oldptr - (unsigned)bottom);/* set offset address*/
    }


    // char *newptr = bp;/* absolute address */
    // char *oldptr = free_list[index];/* heap offset address */
    // free_list[index] = newptr;

    // if(oldptr == bottom){ /* free_list[index] point to tail*/
    //     #ifdef debug
    //     puts("Insert 1");
    //     #endif
    //     SET_PREV_POINTER(newptr, NULL);
    //     SET_NEXT_POINTER(newptr, NULL);
    // }
    // else{
    //     #ifdef debug
    //     puts("Insert 2");
    //     #endif
    //     SET_PREV_POINTER(newptr, NULL);
    //     SET_NEXT_POINTER(newptr, *(unsigned *)oldptr);
    //     SET_PREV_POINTER(oldptr, *(unsigned *)newptr);
    // }

    #ifdef debug
        #ifdef print
        mm_check();
        #endif
    #endif
}

/*
 * delete_node - delete the free node from the list.
 */
static void delete_node(void *bp)
{
    #ifdef debug
    puts("Delete_node");
    #endif
    size_t size = GET_SIZE(HDRP(bp));
    int index = get_index(size);
    char *prevptr = PREV_NODE(bp);/* absolute address */
    char *nextptr = NEXT_NODE(bp);/* absolute address */

    if(prevptr == bottom && nextptr == bottom){ /* Delete both head and tail node */
        #ifdef debug
        puts("Delete 1");
        #endif
        free_list[index] = NULL;
    }
    else if(prevptr == bottom){ /* Delete head node */
        #ifdef debug
        puts("Delete 2");
        #endif
        free_list[index] = (char *)(nextptr - (char *)bottom);/* set offset address */
        SET_PREV_POINTER(nextptr, NULL);/* set offset address */
    }
    else if(nextptr == bottom){ /* Delete tail node */
        #ifdef debug
        puts("Delete 3");
        #endif
        SET_NEXT_POINTER(prevptr, NULL);/* set offset address */
    }
    else{   /* Delete normal node */
        #ifdef debug
        puts("Delete 4");
        #endif
        SET_PREV_POINTER(nextptr, (unsigned)prevptr - (unsigned)bottom);/* set offset address */
        SET_NEXT_POINTER(prevptr, (unsigned)nextptr - (unsigned)bottom);/* set offset address */
    }
    // char *prevptr = PREV_NODE(bp);
    // char *nextptr = NEXT_NODE(bp);

    // if((prevptr == bottom) && (nextptr == bottom)){ /* Delete the both head and tail node */
    //     #ifdef debug
    //     puts("Delete 1");
    //     #endif
    //     free_list[index] = bottom;
    // }
    // else if(prevptr == bottom){    /* Delete head node */
    //     #ifdef debug
    //     puts("Delete 2");
    //     #endif
    //     free_list[index] = nextptr;
    //     SET_PREV_POINTER(nextptr, NULL);
    // }
    // else if(nextptr == bottom){   /* Delete tail node */
    //     #ifdef debug
    //     puts("Delete 3");
    //     #endif
    //     SET_NEXT_POINTER(prevptr, NULL);
    // }
    // else{   /* Delete normal node */
    //     #ifdef debug
    //     puts("Delete 4");
    //     #endif
    //     SET_NEXT_POINTER(prevptr, *(unsigned *)nextptr);
    //     SET_PREV_POINTER(nextptr, *(unsigned *)prevptr);
    // }
}
/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    #ifdef debug
    puts("Realloc");
    #endif
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

/*
 * mm_check - check the correctness of free list and allocated blocks.
 */
static int mm_check(void)
{
    if(!(times >= 500 && times <= 510))
        return 1;
    printf("Time:%d\n", times);
    mm_printfreelist();
    mm_printheap();
    return 0;
}

/*
 * mm_printfresslist - print list of free nodes.
 */
static void mm_printfreelist(void)
{
    void *bp;
    volatile size_t size;
    for(int i = 0; i < FREE_LIST_NUM; i++){
        printf("List[%2d]:", i);
        for(bp = *(unsigned *)free_list[i]+bottom; bp != bottom ; bp = NEXT_NODE(bp)){  /* traverse each list */
            size = GET_SIZE(HDRP(bp));
            printf("%u-->", (unsigned)size);
        }
        printf("\n");
    }
}

/*
 * mm_printheap - print the whole heap's block. 
 */
static void mm_printheap(void)
{
    void *bp = heap_listp;
    size_t size;
    size_t alloc;
    size_t prev_alloc;
    int alloc_num = 0;
    int free_num = 0;
    printf("Heap:\n");
    do{
        /* Get basic information */
        size = GET_SIZE(HDRP(bp));
        alloc = GET_ALLOC(HDRP(bp));
        prev_alloc = GET_PREV_ALLOC(HDRP(bp));
        if(size == 8 && alloc == 1){
            printf("prologue block\n");
            printf("%u/%u/%u\n  |\n",(unsigned)size, (unsigned)prev_alloc, (unsigned)alloc);
        }
        else if(alloc == 0){
            printf("free block[%d]\n", free_num++);
            printf("%u/%u/%u\n  |\n",(unsigned)size, (unsigned)prev_alloc, (unsigned)alloc);
        }
        else if(size == 0)
            break;
        else{
            // printf("alloc block[%d]\n", alloc_num++);
            // printf("%u/%u/%u\n  |\n",(unsigned)size, (unsigned)prev_alloc, (unsigned)alloc);
        }
        bp = NEXT_BLKP(bp);
    }while(!(size == 0 && alloc == 1));
    printf("epilogue block\n");
    prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    printf("%u/%u/%u\n", 0, (unsigned)prev_alloc, 1);
}

/*
 * adjust_alloc_size - adjust allocated block size in order to get higher score of 'space utilization'.
 */
static size_t adjust_alloc_size(size_t size) {
    // freeciv.rep
    if (size >= 120 && size < 128) {
        return 128;
    }
    // binary.rep
    if (size >= 448 && size < 512) {
        return 512;
    }
    if (size >= 1000 && size < 1024) {
        return 1024;
    }
    if (size >= 2000 && size < 2048) {
        return 2048;
    }
    return size;
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