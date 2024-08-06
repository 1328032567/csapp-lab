#include "csapp.h"
#include "sbuf.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400
/* Define threads number */
#define NTHREADS 4
/* Define buffer number */
#define SBUFSIZE 16

/* Define relative data structure */
typedef char string[MAXLINE];
typedef struct{
    string host;    /* URL hostname */
    string port;    /* URL server port */
    string path;    /* URL file path */
}URL;
/* Define shared buffer for connected descriptors*/
sbuf_t sbuf;                

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

/* Define thread functions to deal with connected socket */
void *thread(void *vargp);

void web_proxy(int connfd);
void parseUrl(const string *uri, URL *url);
void getRequest(int connfd, URL *url, string *http_request);

int main(int argc, char **argv) 
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;  /* thread id */

    /* Ignore SIGPIPE signal to develop proxy robustness */
    signal(SIGPIPE, SIG_IGN);

    /* Check command line args */
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = Open_listenfd(argv[1]);
    sbuf_init(&sbuf, SBUFSIZE);
    for(int i = 0;i < NTHREADS; i++)   /* Create worker threads */
        Pthread_create(&tid, NULL, thread, NULL);

    while(1){
        clientlen = sizeof(struct sockaddr_storage);
        connfd = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        sbuf_insert(&sbuf, connfd); /* Insert connfd in buffer */
    }
    return 0;
}

/*
 * thread - threads fetch connected socket from buffer.
 */
void *thread(void *vargp)
{
    Pthread_detach(pthread_self());
    while(1){
        int connfd = sbuf_remove(&sbuf);    /* Remove connfd from buffer */
        web_proxy(connfd);
        Close(connfd);
    }
}

/*
 * web_proxy - get client request, parser it and send to server, finally return response to client.
 */
void web_proxy(int connfd)
{
    string line;
    string http_request;
    URL url;
    int serverfd;
    rio_t rio;
    
    getRequest(connfd, &url, &http_request);

    if((serverfd = Open_clientfd(url.host, url.port)) < 0)
        fprintf(stderr, "Web Proxy connected to server failed.\n");
    
    Rio_readinitb(&rio, serverfd);
    Rio_writen(serverfd, http_request, strlen(http_request));
    
    int n;
    while ((n = Rio_readnb(&rio, line, MAXLINE)) > 0)
        Rio_writen(connfd, line, n);
    
    Close(serverfd);
}

/*
 * getRequest - get the client http request and handle it.
 */
void getRequest(int connfd, URL *url, string *http_request) 
{
    rio_t rio;
    string method, uri, version;
    string buf;
    string other;
    char host[2*MAXLINE];   /* Allocate double size in order to avoid copy overflow */

    Rio_readinitb(&rio, connfd);
    Rio_readlineb(&rio, buf, MAXLINE);
    sscanf(buf, "%s %s %s\n", method, uri, version);
    parseUrl(&uri, url);

    sprintf(host, "Host: %s\r\n", url->host);
    /* Handle request header from client */
    while (Rio_readlineb(&rio, buf, MAXLINE) > 0) {
        if (strcmp(buf, "\r\n") == 0) 
            break;
        if (strncmp(buf, "Host", 4) == 0) 
            strcpy(host, buf);
        if (strncmp(buf, "User-Agent", 10) &&
            strncmp(buf, "Connection", 10) &&
            strncmp(buf, "Proxy-Connection", 16)) 
                strcat(other, buf);
    }
    /* Add all information into host string */
    sprintf(*http_request, "%s %s HTTP/1.0\r\n"
                     "%s"
                     "%s"
                     "Connection: close\r\n"
                     "Proxy-Connection: close\r\n"
                     "%s\r\n", 
                     method, url->path, host, user_agent_hdr, other);
}
/*
 * parserUrl - parser the uri into three parts: hostname, port and path.
 */
void parseUrl(const string *uri, URL *url) 
{
    /* Copy uri to uri_copy*/
    string uri_copy;
    strcpy(uri_copy, *uri);
    /* Point to uri_copy */
    char *s = uri_copy;
    char *ptr = strstr(s, "//");
    if (ptr != NULL) 
        s = ptr + 2;
    else
        fprintf(stderr, "Wrong HTTP format.\n");

    /* Get file path */
    ptr = strchr(s, '/');
    if (ptr != NULL) {
        strcpy(url->path, ptr);
        *ptr = '\0';
    }
    
    /* Get port number */
    ptr = strchr(s, ':');
    if (ptr != NULL) {
        strcpy(url->port, ptr + 1);
        *ptr = '\0';
    } 
    else 
        strcpy(url->port, "80");

    /* Get host name */
    strcpy(url->host, s);
}
