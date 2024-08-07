#include "csapp.h"
#include "cache.h"

/*
 * cache_init - initialize cache structure.
 */
void cache_init(cache_t cp)
{
    for(int i = 0;i < MAX_BLK_NUM; i++){
        cp[i].size = 0;    /* Initialize buffer size */
        cp[i].stamp = 0;   /* Initialize time stamp */
        cp[i].valid = 0;   /* Free block flag bit */
        cp[i].readcnt = 0; /* Clear reader number */
        cp[i].buf = Malloc(MAX_OBJECT_SIZE);           /* Allocate buffer size */
        memset(cp[i].buf, 0, MAX_OBJECT_SIZE);       /* Initialize buffer */

        Sem_init(&cp[i].mutex, 0, 1);  /* Initlially, mutex is 0 */
        Sem_init(&cp[i].w, 0, 1);      /* Initlially, only 1 writer can write */
    }

}

/*
 * cache_update - update each object's time stamp.
 */
void cache_update(cache_t cp)
{
    /* Modify each block's time stamp */
    for(int i = 0;i < MAX_BLK_NUM; i++){
        enter_writer(&cp[i]);
        /* Critical section */        

        cp[i].stamp++; 

        exit_writer(&cp[i]);
    }
}

/*
 * cache_find - find a free cache block.
 */
void cache_find(cache_t cp, int *index)
{
    int maxstamp = 0;
    /* Traverse cache block to find a free block */
    for(int i = 0;i < MAX_BLK_NUM; i++){
        enter_reader(&cp[i]);
        if(cp[i].valid == 0){  /* Valid free block */
            *index = i;
            exit_reader(&cp[i]);
            break;
        }
        else{   /* No free block */
            /* LRU strategy */
            if(cp[i].stamp > maxstamp){
                *index = i;
                maxstamp = cp[i].stamp;
            }
        }
        exit_reader(&cp[i]);
    }
    return ;
}

/*
 * cache_search - give url and search the cache to hit the block.
 */
void cache_search(cache_t cp, int *index, const URL url)
{
    for(int i = 0;i < MAX_BLK_NUM; i++){
        enter_reader(&cp[i]);
        if((CMPURL(cp[i].url, url) == 0) && (cp[i].valid == 1)){   /* hit the cache */
            *index = i;
            exit_reader(&cp[i]);

            enter_writer(&cp[i]);
            cp[i].stamp = 0;
            exit_writer(&cp[i]);

            return;
        }
        exit_reader(&cp[i]);
    }
    /* Cache Miss */
    *index = -1;
    cache_update(cp);
    return;
}

/*
 * cache_insert - insert object block by index.
 */
void cache_insert(cache_t cp, int index, const char *buf, 
                const URL *url, const int size)
{
    enter_writer(&cp[index]);
    /* Copy data to block */
    memcpy(cp[index].buf, buf, MAX_OBJECT_SIZE);
    memcpy(&cp[index].url, url, sizeof(URL));
    cp[index].size = size;   /* Set buffer size */
    cp[index].stamp = 0;     /* Reset time stamp */
    cp[index].valid = 1;     /* Set free valid bit */
    exit_writer(&cp[index]);

    /* Update block time stamp */
    cache_update(cp);
}

/*
 * enter_reader - Enter reader's critical section.
 */
void enter_reader(obj_t *obj)
{
    /* Enter critical section */
    P(&obj->mutex);
    obj->readcnt++;
    if (obj->readcnt == 1)  /* First in */
        P(&obj->w);
    V(&obj->mutex);
    /***********************
     *  Critical Section   *
     **********************/
}

/*
 * exit_reader - Exit from reader's critical section. 
*/
void exit_reader(obj_t *obj)
{
    /***********************
     *  Critical Section   *
     **********************/
    /* Exit from critical section */
    P(&obj->mutex);
    obj->readcnt--;
    if (obj->readcnt == 0)   /* Last out*/
        V(&obj->w);
    V(&obj->mutex);
}

/*
 * enter_writer - enter writer's critical section.
 */
void enter_writer(obj_t *obj)
{
    P(&obj->w);
    /***********************
     *  Critical Section   *
     **********************/
}

/*
 * exit_writer - exit from writer's critical section.
 */
void exit_writer(obj_t *obj)
{
    /***********************
     *  Critical Section   *
     **********************/
    V(&obj->w);
}

/* Global variables */
int readcnt;    /* Initially = 0 */
sem_t mutex, w; /* Both initially = 1 */

void reader(void)
{
    while(1){
        P(&mutex);
        readcnt++;
        if (readcnt == 1)  /* First in */
            P(&w);
        V(&mutex);

        /* Crictical section */


        P(&mutex);
        readcnt--;
        if (readcnt == 0)   /* Last one */
            V(&w);
        V(&mutex);
    }
}

void writer(void)
{
    while(1){
        P(&w);
        
        /* Critical section */

        V(&w);
    }
}