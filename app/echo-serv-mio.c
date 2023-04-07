#include <csapp.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int maxfd;        /* largest descriptor in read_set, property */
  fd_set read_set;  /* set of all active descriptors, property */
  fd_set ready_set; /* subset of descriptors ready for reading, property */
  int nready;       /* number of ready descriptors, property */
  int maxi;         /* high water index into client array, property */

  int clientfd[FD_SETSIZE];    /* set of active descriptors */
  rio_t clientrio[FD_SETSIZE]; /* set of active read buffer */
} Pool;

int byte_cnt = 0;

void init_pool(int listenfd, Pool *pool);
void add_client(int connfd, Pool *pool);
void check_clients(Pool *pool);

/*!
 * @brief main
 *
 * in infinite loop of server, select function detects two different kinds of
 * input events: 1. a connection request arriving from a new client, and 2. a
 * connected descriptor for an existing client being ready for reading.
 */
int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(0);
  }

  int listenfd = open_listenfd(argv[1]);
  static Pool pool;
  init_pool(listenfd, &pool);

  socklen_t clientlen;
  struct sockaddr_storage clientaddr;
  int connfd, rc;
  char client_hostname[MAXLINE], client_port[MAXLINE];

  while (1) {
    pool.ready_set = pool.read_set; /* IMPORTANT, read set may be updated */
    // return the total number of bit set in the ready set
    pool.nready = select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);

    // if listening descriptor is ready, add new-arriving client to the pool
    if (FD_ISSET(listenfd, &pool.ready_set)) {
      clientlen = sizeof(struct sockaddr_storage);
      connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

      if ((rc = getnameinfo((struct sockaddr *)&clientaddr, clientlen,
                            client_hostname, MAXLINE, client_port, MAXLINE,
                            0) != 0)) {
        fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(rc));
        exit(1);
      };
      printf("Connected to (%s, %s) with connfd(%d)\n", client_hostname,
             client_port, connfd);

      add_client(connfd, &pool);
    }

    // echo a SINGLE text line from each ready connected descriptor
    check_clients(&pool);
  }
}

void init_pool(int listenfd, Pool *pool) {
  memset(pool, 0, sizeof(Pool));

  // no connected descriptor initially
  pool->maxi = -1;
  for (int i = 0; i < FD_SETSIZE; i++) {
    pool->clientfd[i] = -1;
  }

  // initially, listenfd is the only member of select read set
  pool->maxfd = listenfd;
  FD_ZERO(&pool->read_set);
  FD_SET(listenfd, &pool->read_set);
}

void add_client(int connfd, Pool *p) {
  // listenfd is ready when a new client comes, remove listenfd from ready count
  p->nready--;
  // find an available slot
  int i;
  for (i = 0; i < FD_SETSIZE; i++) {
    if (p->clientfd[i] < 0) {
      // add connfd to the pool
      p->clientfd[i] = connfd;
      rio_readinitb(&p->clientrio[i], connfd);

      // add descriptor to the descriptor set
      FD_SET(connfd, &p->read_set);

      // update max descriptor and pool high water index
      if (connfd > p->maxfd) {
        p->maxfd = connfd;
      }
      if (i > p->maxi) {
        p->maxi = i;
      }

      printf("add_client: connfd(%d) is added to client[%d]\n", connfd, i);
      break;
    }
  }
  if (i == FD_SETSIZE) {
    fprintf(stderr, "add_client error: too many clients\n");
    exit(-1);
  }
}

void check_clients(Pool *p) {
  int n;
  char buf[MAXLINE];

  for (int i = 0; (i <= p->maxi) && (p->nready > 0); i++) {
    int connfd = p->clientfd[i]; /* -1 by default */
    rio_t rio = p->clientrio[i];

    if ((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))) {
      p->nready--;
      // read a SINGLE line each time from connfd
      if ((n = rio_readlineb(&rio, buf, MAXLINE)) != 0) {
        byte_cnt += n;
        printf("Server received %d (%d total) bytes on connfd(%d)\n", n,
               byte_cnt, connfd);
        // echo back
        rio_writen(connfd, buf, n);
      } else {
        // eof detected, remove descriptor from the pool
        close(connfd);
        FD_CLR(connfd, &p->read_set);
        p->clientfd[i] = -1; // the slot is free

        printf("INFO: connfd(%d) disconnected and client[%d] is cleared\n",
               connfd, i);
      }
    }
  }
}
