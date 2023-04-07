#include "inet-helper.h"
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/*!
 * @brief open_clientfd tries to return a connect socket for given
 * hostname:port
 *
 * return clientfd on success, -1 otherwise
 */
int open_clientfd(char *hostname, char *port) {
  struct addrinfo hints, *listp;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM; /* open a connection socket */
  hints.ai_flags = AI_NUMERICSERV; /* using a numeric port argument */
  hints.ai_flags |= AI_ADDRCONFIG; /* recommended for connections */

  int rc;
  if ((rc = getaddrinfo(hostname, port, &hints, &listp) != 0)) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
    exit(1);
  };

  int clientfd;
  struct addrinfo *p;
  for (p = listp; p; p = p->ai_next) {
    if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
      continue;
    }
    if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1) {
      // success on connection
      break;
    }
    // close clientfd if it fails to connect to the server
    close(clientfd);
  }

  freeaddrinfo(listp);
  if (p == NULL)
    // if all connects failed
    return -1;
  else
    return clientfd;
}

/*!
 * @brief open_listenfd tries to create a listenfd for a server exposed with
 * port number 'port'
 *
 * return listenfd on success, -1 otherwise
 */
int open_listenfd(char *port) {
  struct addrinfo hints, *listp;

  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_socktype = SOCK_STREAM; /* accecpt connections */
  hints.ai_flags =
      AI_PASSIVE | AI_ADDRCONFIG; /* passive socket and listen on any address */
  hints.ai_flags |= AI_NUMERICSERV; /* using port number */

  int rc;
  if ((rc = getaddrinfo(NULL, port, &hints, &listp) != 0)) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
    exit(1);
  };

  int listenfd, optval = 1;
  struct addrinfo *p;
  for (p = listp; p; p = p->ai_next) {
    if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0) {
      // socket failed, try next
      continue;
    }
    // eliminate "address already in use" error from bind
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval,
               sizeof(int));
    // assign a name to a socket: bind the discriptor to the address
    if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
      // break on success
      break;
    }
    close(listenfd);
  }

  freeaddrinfo(listp);
  if (!p)
    return -1;

  // make it a listening socket ready to accept connections request
  if (listen(listenfd, LISTENQ) < 0) {
    close(listenfd);
    return -1;
  }

  return listenfd;
}
