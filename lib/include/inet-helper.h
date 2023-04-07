#pragma once
#ifndef INET_HELPER_H
#define INET_HELPER_H

#define LISTENQ 1024  /* listen queue size */

// return socket fd if OK, -1 on error
int open_clientfd(char *hostname, char *port);
// return lisen socket fd if OK, -1 on error
int open_listenfd(char *port);

#endif  /* INET_HELPER_H */
