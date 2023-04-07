// #define NDEBUG // disables assert()
#include <assert.h>
#include <csapp.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
    exit(0);
  }

  char *host = argv[1];
  char *port = argv[2];
  int clientfd = open_clientfd(host, port);

  rio_t rio; /* an io stream object */
  Rio_readinitb(&rio, clientfd);
  char buf[MAXLINE];

  /*!
   * @brief stdin => server
   *        stdin => output
   */
  while (fgets(buf, MAXLINE, stdin) != NULL) {
    // write to clientfd whose content will be sent to server
    Rio_writen(clientfd, buf, strlen(buf));
    // output the content of io object that's associated with clientfd to stdin
    Rio_readlineb(&rio, buf, MAXLINE);
    fputs(buf, stdout);
  }

  close(clientfd);

  return 0;
}
