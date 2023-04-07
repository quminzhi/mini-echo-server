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

void sigchld_handler(int sig) {
  // reap all terminated worker processes
  pid_t pid;
  int status, rc;
  while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
    if (WIFEXITED(status) || WIFSIGNALED(status)) {
      printf("sigchld_handler: worker process (%d) terminated OK (status %d) "
             "and reaped normally\n",
             pid, WEXITSTATUS(status));
    }
  }
  return;
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(0);
  }

  // install sigchld handler
  signal(SIGCHLD, sigchld_handler);

  int listenfd = open_listenfd(argv[1]);

  int connfd, rc;
  struct sockaddr_storage
      clientaddr; /* enough space for any address, used to save the address of
                     peer socket (client) */
  socklen_t clientlen = sizeof(struct sockaddr_storage);
  char client_hostname[MAXLINE];
  char client_port[MAXLINE];
  pid_t pid;

  while (1) {
    // accept() will suspend current process
    // it extracts the first connection request on the queue of pending
    // connections for the listening socket, creates a new connected socket, and
    // returns a new fd referring to that socket.
    connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

    // fork a worker process for connfd
    if ((pid = fork()) == 0) {
      close(listenfd); /* child closes listenfd in its file descriptor table */
      echo(connfd);
      close(connfd);
      exit(0);
    }

    close(connfd); /* parent closes connfd of its file descriptor table */

    // output schedule info
    if ((rc = getnameinfo((struct sockaddr *)&clientaddr, clientlen,
                          client_hostname, MAXLINE, client_port, MAXLINE,
                          0) != 0)) {
      fprintf(stderr, "getnameinfo error: %s\n", gai_strerror(rc));
      exit(1);
    };

    printf("Connected to (%s, %s) and assigned to worker process(%d)\n",
           client_hostname, client_port, pid);
  }

  return 0;
}
