#include <hostinfo.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <domain name>\n", argv[0]);
    exit(0);
  }

  print_info(argv[1]);

  return 0;
}
