#include <csapp.h>

/*! TODO: reader-writer problem which favors reader.
 *
 * @todo reader has higher priority than writer. Writer is allowed to write iff
 * there is no reader.
 */

int readcnt = 0;
sem_t mutex; /* protects readcnt */
sem_t w;     /* writer lock */

void reader() {
  while (1) {
    P(&mutex);
    readcnt++;
    if (readcnt == 1) {
      // lock writer when the first reader in
      P(&w);
    }
    V(&mutex);

    // critical section: reader favored
    printf("reading ...\n");
    // critical section

    P(&mutex);
    readcnt--;
    if (readcnt == 0) {
      V(&w);
    }
    V(&mutex);
  }
}

void writer() {
  while (1) {
    P(&w);
    printf("writing ...\n");
    V(&w);
  }
}

int main(int argc, char *argv[]) { 
  
  return 0; 
}
