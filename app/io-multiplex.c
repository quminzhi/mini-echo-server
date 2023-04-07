#include <csapp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

/*! TODO: This code showcases how to use select for I/O multiplexing
 *
 * @todo an echo server which communicates with client and respond to the user
 * input from stdin at the same time.
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

void command() {
  char buf[MAXLINE];
  if (fgets(buf, MAXLINE, stdin) != 0) {
    // just output line read from stdin for simplicity
    printf("%s", buf);
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

  fd_set read_set;                 /* provided for select */
  fd_set ready_set;                /* check status */
  FD_ZERO(&read_set);              /* clear read set */
  FD_SET(STDIN_FILENO, &read_set); /* add stdin to read set */
  FD_SET(listenfd, &read_set);     /* add listenfd to read set */

  while (1) {
    ready_set = read_set;
    // suspend current process until a read bit is set
    // @1st param: length of set
    select(listenfd + 1, &ready_set, NULL, NULL, NULL);

    // check which bit is set in ready_set
    if (FD_ISSET(STDIN_FILENO, &ready_set)) {
      command();
    }
    if (FD_ISSET(listenfd, &ready_set)) {
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
  }

  return 0;
}
