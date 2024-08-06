#include "csapp.h"
#include "sbuf.h"

/* Define string type to simplify */
typedef char string[MAXLINE];
/* Define threads number */
#define NTHREADS 4
/* Define buffer number */
#define SBUFSIZE 16

typedef struct{
    string host;    /* URL hostname */
    string port;    /* URL server port */
    string path;    /* URL file path */
}URL;

/* Define thread functions to deal with connected socket */
void *thread(void *vargp);

void web_proxy(int connfd);
void parseUrl(const string *uri, URL *url);
void getRequest(int connfd, URL *url, string *http_request);
