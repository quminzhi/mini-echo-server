#include <hostinfo.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define HOST_LEN 100
#define SERV_LEN 100

void print_info(const char *hostname) {
  struct addrinfo *listp, hints;
  
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET; /* ipv4 only */
  hints.ai_socktype = SOCK_STREAM; /* connection socket only */

  int rc;
  if ((rc = getaddrinfo(hostname, NULL, &hints, &listp)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
    exit(1);
  }

  /*! TODO: walk the list and display each IP address
   *
   * @todo iterate over the result list returned (listp)
   */
  int flags = NI_NUMERICHOST; /* display address string instead of domain name */

  char host[HOST_LEN];
  char serv[SERV_LEN];
  for (struct addrinfo *p = listp; p; p = p->ai_next) {
    getnameinfo(p->ai_addr, p->ai_addrlen, host, HOST_LEN, serv, SERV_LEN, flags);
    printf("host = %s, service = %s\n", host, serv);
  }

  // dynamically allocated by getaddrinfo
  freeaddrinfo(listp);
}
