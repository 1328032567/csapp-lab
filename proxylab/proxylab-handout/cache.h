#include "csapp.h"
#include "proxy.h"

/* Define compare url's macro */
#define CMPURL(url1, url2)     (strcmp(url1.host, url2.host) || \
                                strcmp(url1.port, url2.port) || \
                                strcmp(url1.path, url2.path))
/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
#define MAX_BLK_NUM     (int)(MAX_CACHE_SIZE/MAX_OBJECT_SIZE)

/* Define cache object data structure */
typedef struct{
    char *buf;                  /* Cache block array */
    URL url;                    /* Object identifier */
    int size;                   /* Object size */
    int stamp;                  /* Time stamp */
    int valid;                  /* Record block usage */

    int readcnt;                /* Reader's number */
    sem_t mutex;                /* Lock on write readcnt */
    sem_t w;                    /* Lock on write block */
}obj_t;

/* Define cache structure */
typedef obj_t cache_t[MAX_BLK_NUM]; /* cache own MAX_BLK_NUM blocks */

void cache_init(cache_t cp);
void cache_update(cache_t cp);
void cache_insert(cache_t cp, int index, const char *buf, 
                const URL *url, const int size);
void cache_find(cache_t cp, int *index);
void cache_search(cache_t cp, int *index, const URL url);
void cache_deinit(cache_t cp);


void enter_reader(obj_t *obj);
void exit_reader(obj_t *obj);
void enter_writer(obj_t *obj);
void exit_writer(obj_t *obj);

void reader(void);
void writer(void);