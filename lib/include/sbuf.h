#pragma once
#ifndef SBUF_H
#define SBUF_H

#include <semaphore.h>
#include <csapp.h>

typedef struct {
  int *buf;    /* bounded buffer array */
  int n;       /* maximum number of slots */
  int front;   /* buf[(front + 1) % n] is first item */
  int rear;    /* buf[rear % n] is the last item, (front, rear] */
  sem_t mutex; /* protects access to buffer */
  sem_t slots; /* counts available slots */
  sem_t items; /* counts available items */
} sbuf_t, *sbuf_ptr_t;

void sbuf_init(sbuf_ptr_t sp, int n);
void sbuf_teardown(sbuf_ptr_t sp);
void sbuf_insert(sbuf_ptr_t sp, int item); /* insert into the rear */
int sbuf_remove(sbuf_ptr_t sp); /* remove from the front */

#endif /* SBUF_H */
