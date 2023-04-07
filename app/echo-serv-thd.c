#include <csapp.h>
#include <stdio.h>
#include <stdlib.h>

/*! TODO: thread version of echo server
 *
 * @todo create worker threads for each request
 *  - how to pass argument (malloc to prevent RACE)
 *  - how to reap thread resources (kernel)
 */ 

void echo(int connfd) {
  size_t n;
  char buf[MAXLINE];

  // associate rio with connfd
  rio_t rio;
  rio_readinitb(&rio, connfd);

  while ((n = rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    printf("server received %d bytes\n", (int)n);
    // write into connfd, echo back to the client
    rio_writen(connfd, buf, n);
  }
}

void *worker(void *vargp) {
  int connfd = *((int *)vargp);
  pthread_detach(pthread_self()); /* make it a detached thread */
  free(vargp);                    /* free allocated space in main thread */
  echo(connfd);
  close(connfd);

  printf("Bye bye ~ from thread(%lu)\n", pthread_self());

  return NULL;
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(0);
  }

  int listenfd = open_listenfd(argv[1]);

  int *connfdp, rc;
  struct sockaddr_storage
      clientaddr; /* enough space for any address, used to save the address of
                     peer socket (client) */
  socklen_t clientlen = sizeof(struct sockaddr_storage);
  char client_hostname[MAXLINE];
  char client_port[MAXLINE];
  pthread_t tid;

  while (1) {
    connfdp = (int *)malloc(sizeof(int));  /* prevent RACE */
    *connfdp = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

    pthread_create(&tid, NULL, worker, connfdp);

    // output schedule info
    if ((rc = getnameinfo((struct sockaddr *)&clientaddr, clientlen,
                          client_hostname, MAXLINE, client_port, MAXLINE,
                          0) != 0)) {
      fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(rc));
      exit(1);
    };

    printf("Connected to (%s, %s) and assigned to worker thread(%lu)\n",
           client_hostname, client_port, tid);
  }

  return 0;
}
