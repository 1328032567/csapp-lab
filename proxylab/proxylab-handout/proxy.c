#include "csapp.h"
#include "sbuf.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

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

sbuf_t sbuf;                /* Shared buffer of connected descriptors*/
/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

/* Define thread functions to deal with connected socket */
void *thread(void *vargp);

void web_proxy(int fd);
int get_request(int fd, string* http_request, string* host, string* port);
void read_requesthdrs(rio_t *rp, string *header, string *hostname);
int parse_uri(string *uri, URL *url);
/* Define client error function to deliver error code back */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    /* Ignore SIGPIPE signal to develop proxy robustness */
    signal(SIGPIPE, SIG_IGN);
    if(argc != 2){
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
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
void web_proxy(int fd)
{
    string http_request;
    string host, port;
    string buf;
    int serverfd;
    rio_t rio;

    if(get_request(fd, &http_request, &host, &port) != 0){
        clienterror(fd, http_request, "505", "Not supported", "Web proxy can't parser request successfully");
        return;
    }

    /* Web Proxy connected to server */
    if((serverfd = open_clientfd(host, port)) < 0){/* Connect failed */
        fprintf(stderr, "Web Proxy connected to server failed.\n");
        return;
    }

    printf("%s", http_request);

    /* Send HTTP Request to Server */
    rio_readinitb(&rio, serverfd);
    Rio_writen(serverfd, http_request, strlen(http_request));

    int n;
    while((n = Rio_readnb(&rio, buf, MAXLINE)) > 0)
        Rio_writen(fd, buf, n);
    Close(serverfd);
}
/*
 * get_request - get http request which is consist of request line part and request header part, then parser it.
 */
int get_request(int fd, string* http_request, string* host, string* port)
{
    string buf, method, uri, version, header;
    URL url;
    rio_t rio;

    rio_readinitb(&rio, fd);
    /* Read in http request line */
    if(rio_readlineb(&rio, buf, MAXLINE) <= 0){
        fprintf(stderr, "Read request line error: %s\n", strerror(errno));
        return -1;
    }
    sscanf(buf, "%s %s %s", method, uri, version);
    if(strcasecmp(method, "GET")){
        clienterror(fd, method, "501", "Not implemented", "Web Proxy does not implement this method");
        return -1;
    }
    /* Parser url into 3 parts: host, port, path */
    if(parse_uri(&uri, &url) != 0){
        fprintf(stderr, "Not the HTTP protocal.\n");
        return -1;
    }
    /* Read in http request header and parser it */
    read_requesthdrs( &rio, &header, &url.host);
    /* Generate Server host and port */
    strcat(*host, url.host);
    strcat(*port, url.port);
    /* Generate HTTP Request from Web Proxy */
    sprintf(*http_request,    "%s %s HTTP/1.0\r\n"
                                        "%s", 
                                        method, url.path, header);
    return 0;
}
/*
 * read_requesthdrs - read in client http request header.
 */
void read_requesthdrs(rio_t *rp, string *header, string *hostname)
{
    string buf;
    string host, other;

    sprintf(host, "Host: %s\r\n", *hostname);
    /* Handle request header from client */
    while(rio_readlineb(rp, buf, MAXLINE) > 0){
        if(strcmp(buf, "\r\n") == 0)
            break;
        if(strncmp(buf, "Host", 4) == 0)
            strcpy(host, buf);
        if(strncmp(buf, "User-Agent", 10) && strncmp(buf, "Connection", 10)
            && strncmp(buf, "Proxy-Connection", 16))
            strcat(other, buf);
    }
    /* Add all information into host string */
    sprintf(*header,  "%s%s"
                                "Connection: close\r\n"
                                "Proxy-Connection: close\r\n"
                                "%s"
                                "\r\n", /* terminated by an empty line */
                                host, user_agent_hdr, other);
}
/*
 * parse_uri - parser uri into url.
 */
int parse_uri(string *uri, URL *url)
{
    char *s = *uri;
    char *ptr = strstr(s, "//");
    if (ptr != NULL) s = ptr + 2;
 
    ptr = strchr(s, '/');
    if (ptr != NULL) {
        strcpy(url->path, ptr);
        *ptr = '\0';
    }
    
    ptr = strchr(s, ':');
    if (ptr != NULL) {
        strcpy(url->port, ptr + 1);
        *ptr = '\0';
    } else strcpy(url->port, "80");
 
    strcpy(url->host, s);
    return 0;
}
/*
 * clienterror - deal with error code and message.
 */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg)
{
    char buf[MAXLINE];

    /* Print the HTTP response headers */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n\r\n");
    Rio_writen(fd, buf, strlen(buf));

    /* Print the HTTP response body */
    sprintf(buf, "<html><title>Web Proxy Error</title>");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<body bgcolor=""ffffff"">\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "<hr><em>The Web Proxy</em>\r\n");
    Rio_writen(fd, buf, strlen(buf));
}
