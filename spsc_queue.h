#ifndef SPSC_QUEUE_H
#define SPSC_QUEUE_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <stdatomic.h>

typedef struct spsc_queue {

  int *buf;
  int pwrite;
  int pread;
  size_t size;

} spsc_queue;

int spsc_queue_init(spsc_queue *queue, size_t size) {

  queue->buf = malloc(sizeof(int) * size);
  queue->size = size;

  return 0;
}

int spsc__queue_free(spsc_queue *queue) {
  free(queue->buf);
  return 0;
}

int spsc_queue_enqueue(spsc_queue *queue, int *value) {

  if (atomic_load_explicit(&queue->buf[queue->pwrite], memory_order_acquire)) {

    atomic_store_explicit(&queue->buf[queue->pwrite], value,
                          memory_order_release);

    queue->pwrite += (queue->pwrite + 1 >= queue->size) ? (1 - queue->size) : 1;
    return 1;
  }
}

int spsc_queue_dequeue(spsc_queue *queue, int **value) {
  if (!atomic_load_explicit(&queue->buf[queue->pread], memory_order_acquire))
    return 0;

  *value = queue->buf[queue->pread];
  atomic_store_explicit(&queue->buf[queue->pread], NULL, memory_order_release);

  queue->pread += (queue->pread + 1 >= queue->size) ? (1 - queue->size) : 1;
  return 1;
}

#endif