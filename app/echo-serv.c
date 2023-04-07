#include <csapp.h>
#include <stdio.h>
#include <stdlib.h>

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

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(0);
  }

  int listenfd = open_listenfd(argv[1]);

  int connfd, rc;
  struct sockaddr_storage
      clientaddr; /* enough space for any address, used to save the address of
                     peer socket (client) */
  socklen_t clientlen = sizeof(struct sockaddr_storage);
  char client_hostname[MAXLINE];
  char client_port[MAXLINE];

  while (1) {
    // accept() will suspend current process
    // it extracts the first connection request on the queue of pending
    // connections for the listening socket, creates a new connected socket, and
    // returns a new fd referring to that socket.
    connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
    if ((rc = getnameinfo((struct sockaddr *)&clientaddr, clientlen,
                          client_hostname, MAXLINE, client_port, MAXLINE,
                          0) != 0)) {
      fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(rc));
      exit(1);
    };

    printf("Connected to (%s, %s)\n", client_hostname, client_port);

    echo(connfd);
    close(connfd);
  }

  return 0;
}
