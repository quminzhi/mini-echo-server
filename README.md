# Echo Server 

This is an echo server which echo back the input from user. The server is
implemented with different design pattern to achieve concurrency.

## Basic Architecture

At server part, a listen file descriptor is created and used to listen and
accept connection from clients. Every time a connection is accept, a connection
file descriptor (`connfd`) is created, which will be serviced in various ways in
different design pattern.

On client side, a connection file descriptor is created and send request to a
host.

In linux, everything related to I/O is a file, as with net communication.

## Concurrency

In this section, concurrent server is implemented which is able to service many
clients in parallel. It is implemented with three different methods:
process-based server, I/O multiplexing, and thread-based server.

### Process Based Server

The process based design is to create a new process to handle a service request
(`connfd`). The up side is there is a good isolation between different services,
yet it restricted the sharing of information among services.

```c
// fork a child process to provide service
while (1) {
  connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
  // fork a worker process for connfd
  if ((pid = fork()) == 0) {
    close(listenfd); /* child closes listenfd in its file descriptor table */
    echo(connfd);
    close(connfd);
    exit(0);
  }
  close(connfd); /* parent closes connfd of its file descriptor table */
}
```

Do not forget to reap terminated child processes.

```c
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
```

### IO Multiplexing Server

I/O multiplexing is another good design for echo server, which allows us to
control schedule of requests. As it is in the same process with one virtual
memory, sharing info between different request handler becomes much easier. The
downside is obvious as well. The complexity of code increases dramatically and
it CANNOT utilize the power of multi-core processors.

```c
// use a pool to manage scheduling manually
typedef struct {
  int maxfd;        /* largest descriptor in read_set, property */
  fd_set read_set;  /* set of all active descriptors, property */
  fd_set ready_set; /* subset of descriptors ready for reading, property */
  int nready;       /* number of ready descriptors, property */
  int maxi;         /* high water index into client array, property */

  int clientfd[FD_SETSIZE];    /* set of active descriptors */
  rio_t clientrio[FD_SETSIZE]; /* set of active read buffer */
} Pool;

static Pool pool;
init_pool(listenfd, &pool);
```

```c
// the interesting part is how to use select() to figure out active fd and solve
// them with the help of ready set and read set
while (1) {
  pool.ready_set = pool.read_set; /* IMPORTANT, read set may be updated */
  // return the total number of bit set in the ready set
  pool.nready = select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);

  // if listening descriptor is ready, add new-arriving client to the pool
  if (FD_ISSET(listenfd, &pool.ready_set)) {
    clientlen = sizeof(struct sockaddr_storage);
    connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
    add_client(connfd, &pool);
  }

  // echo a SINGLE text line from each ready connected descriptor
  check_clients(&pool);
}
```

### Thread Based server

Thread is a kind of light-weight "process". Each thread has its own thread
context, including a unnique integer thread ID, stack, stack pointer, program
counter, general-purpose registers, and condition codes. In fact, thread is the
minimum unit on which a processor schedule. Different from a process, there is
no child-parent relations among threads.

Similar to process-based approach, a new threaded is created to service a new
request.

```c
void *worker(void *vargp) {
  int connfd = *((int *)vargp);
  pthread_detach(pthread_self()); /* make it a detached thread */
  free(vargp);                    /* free allocated space in main thread */
  echo(connfd);
  close(connfd);

  printf("Bye bye ~ from thread(%lu)\n", pthread_self());

  return NULL;
}
```

```c
// main loopj
while (1) {
  connfdp = (int *)malloc(sizeof(int));  /* prevent RACE */
  *connfdp = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

  pthread_create(&tid, NULL, worker, connfdp);
}
```

### Thread Pool

Even the performance of thread-based solution is pretty satisfying. Can we
optimize futher to overcome the overheads of context switch of threads? Yes, we
can create some threads in advance that are ready to service client requests.

To make it feasible, we need a buffer queue (`sbuf`), a producer-consumer model.
The main thread creates listen fd, accepts client requests, and push `connfd`
into the buffer. Worker threads check if there is any `connfd` in the queue. If
so, retrieve it and service it!

```c
// main thread 
int main(int argc, char *argv) {
  // ...
  int listenfd = open_listenfd(argv[1]);
  sbuf_init(&sbuf, SBUFSIZE);

  pthread_t tid;
  for (int i = 0; i < NTHREADS; i++) {
    pthread_create(&tid, NULL, worker, (void *)(unsigned long)i);
  }

  while (1) {
    connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
    sbuf_insert(&sbuf, connfd);
  }
}
```

```c
// worker side
void *worker(void *vargp) {
  pthread_detach(pthread_self());
  // ...
  while (1) {
    int connfd = sbuf_remove(&sbuf);
    // service
    echo(connfd);
    close(connfd);
  }
}
```

That's it! The buffer queue connect main thread and worker threads.

## Features

- Different design pattern.
- Cover basic concept and technique about multi-process, multi-thread, and I/O
  multiplexing.
- A test driven project.

## Contributing Changes

Contributions are welcomed and please send your changes as the form as pull
request (PR).

Contact: quminzhi@gmail.com
