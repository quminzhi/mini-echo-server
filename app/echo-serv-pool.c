#include <csapp.h>
#include <sbuf.h>
#include <stdio.h>
#include <stdlib.h>

/*! TODO: echo server with connfd buffer and thread pool
 *
 * @todo 4 worker threads try to retrieve connfd from buffer and service a
 * client, and the main thread accept connection and add connfd to the
 * buffer.
 */

#define NTHREADS 4
#define SBUFSIZE 16

sbuf_t sbuf;

void echo(int connfd);
void *worker(void *vargp);

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(0);
  }

  int listenfd = open_listenfd(argv[1]);
  sbuf_init(&sbuf, SBUFSIZE);

  pthread_t tid;
  for (int i = 0; i < NTHREADS; i++) {
    pthread_create(&tid, NULL, worker, (void *)(unsigned long)i);
  }

  int connfd, rc;
  struct sockaddr_storage
      clientaddr; /* enough space for any address, used to save the address of
                     peer socket (client) */
  socklen_t clientlen = sizeof(struct sockaddr_storage);
  char client_hostname[MAXLINE];
  char client_port[MAXLINE];

  while (1) {
    connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
    sbuf_insert(&sbuf, connfd);

    if ((rc = getnameinfo((struct sockaddr *)&clientaddr, clientlen,
                          client_hostname, MAXLINE, client_port, MAXLINE,
                          0) != 0)) {
      fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(rc));
      exit(1);
    };

    printf("INFO: connected to (%s, %s) with connfd(%d)\n", client_hostname,
           client_port, connfd);
  }

  sbuf_teardown(&sbuf);

  return 0;
}

void *worker(void *vargp) {
  pthread_detach(pthread_self());

  unsigned long wid = (unsigned long)vargp;

  printf("INFO: worker[%lu] is created and ready for service\n", wid);

  while (1) {
    // try to get and remove connfd from buffer
    int connfd = sbuf_remove(&sbuf);
    printf("INFO: worker[%lu] retrieved connfd(%d)\n", wid, connfd);
    // service
    echo(connfd);
    close(connfd);
    printf("INFO: worker[%lu] is disconnected from connfd(%d)\n", wid, connfd);
  }
}

static int byte_cnt; /* byte counter */
static sem_t mutex;  /* protects byte_cnt */

static void init_echo() {
  sem_init(&mutex, 0, 1);
  byte_cnt = 0;
}

void echo(int connfd) {
  // INFO: execute init() only once with technique of pthread_once
  static pthread_once_t once = PTHREAD_ONCE_INIT;
  pthread_once(&once, init_echo);

  rio_t rio;
  rio_readinitb(&rio, connfd);

  int n;
  char buf[MAXLINE];
  while ((n = rio_readlineb(&rio, buf, MAXLINE)) != 0) {
    P(&mutex); /* protect byte_cnt */
    byte_cnt += n;
    printf("LOG: server received %d (%d total) bytes on connfd(%d)\n", n, byte_cnt,
           connfd);
    V(&mutex);
    rio_writen(connfd, buf, n);
  }
}
