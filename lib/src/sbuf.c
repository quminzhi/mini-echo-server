#include <sbuf.h>

void sbuf_init(sbuf_ptr_t sp, int n) {
  sp->buf = calloc(n, sizeof(int));
  sp->n = n;
  sp->front = sp->rear = 0;
  sem_init(&sp->mutex, 0, 1); /* sem as lock */
  sem_init(&sp->slots, 0, n); /* sem as counter */
  sem_init(&sp->items, 0, 0);
}

void sbuf_teardown(sbuf_ptr_t sp) { free(sp->buf); }

void sbuf_insert(sbuf_ptr_t sp, int item) {
  P(&sp->slots);
  P(&sp->mutex);
  sp->buf[(++sp->rear) % (sp->n)] = item;
  V(&sp->mutex);
  V(&sp->items); /* increase items not slots */
}

int sbuf_remove(sbuf_ptr_t sp) {
  int removed;
  P(&sp->items); /* wait for available item */
  P(&sp->mutex); /* lock the buffer */
  removed = sp->buf[(++sp->front) % (sp->n)];
  V(&sp->mutex); /* unlock the buffer */
  V(&sp->slots); /* announce available slot */
  return removed;
}

int sbuf_is_empty(sbuf_ptr_t sp) { return sp->front == sp->rear; }
