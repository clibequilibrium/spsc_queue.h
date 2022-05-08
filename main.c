#include "spsc_queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

size_t const THREAD_COUNT = 1;
size_t const BATCH_SIZE = 10;
size_t const ITERATION_COUNT = 5;

int volatile g_start = 0;

__attribute__((always_inline)) static inline uint64_t rdtscp() {
  uint64_t tsc;

  __asm__("rdtscp;"
          "shl $32, %%rdx;"
          "or %%rdx,%%rax"
          : "=a"(tsc)
          :
          : "%rcx", "%rdx", "memory");

  return tsc;
}
__attribute__((always_inline)) static inline void nop() { __asm__("rep nop;"); }

void *test_enqueue(void *context) {
  struct spsc_queue *queue = (struct spsc_queue *)context;

  srand((unsigned)time(0) + 1);
  size_t pause = rand() % 1000;

  /* Wait for global start signal */
  while (g_start == 0)
    sched_yield();

  /* Wait for a random time */
  for (size_t i = 0; i != pause; i += 1)
    nop();

  for (int iter = 0; iter != ITERATION_COUNT; ++iter) {
    for (size_t i = 0; i != BATCH_SIZE; i += 1) {
      spsc_queue_enqueue(queue, (void *)i);
    }
  }
  sched_yield();

  return 0;
}

void *test_dequeue(void *context) {
  struct spsc_queue *queue = (struct spsc_queue *)context;

  srand((unsigned)time(0) + 1);
  size_t pause = rand() % 1000;

  /* Wait for global start signal */
  while (g_start == 0)
    sched_yield();

  /* Wait for a random time */
  for (size_t i = 0; i != pause; i += 1)
    nop();

  for (int iter = 0; iter != ITERATION_COUNT; ++iter) {
    for (size_t i = 0; i != BATCH_SIZE; i += 1) {
      void *ptr;
      while (!spsc_queue_dequeue(queue, &ptr)) {
        continue;
      }
    }
  }

  sched_yield(); // queue empty

  return 0;
}

int main() {
  struct spsc_queue queue;
  pthread_t threads[THREAD_COUNT + 1];
  int exit_code;

  spsc_queue_init(&queue, BATCH_SIZE * ITERATION_COUNT);

  pthread_create(&threads[0], NULL, test_enqueue, &queue);
  pthread_create(&threads[1], NULL, test_dequeue, &queue);

  _sleep(1);

  uint64_t start = rdtscp();
  g_start = 1;

  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);

  uint64_t end = rdtscp();

  printf("cycles/op = %lu\n",
         (end - start) / (BATCH_SIZE * ITERATION_COUNT * 2 * THREAD_COUNT));

  if (queue.pread != queue.pread)
    printf("Queue is not empty.\n");


   printf("read %d write %d\n", queue.pread, queue.pwrite);

  exit_code = spsc__queue_free(&queue);
  if (exit_code)
    printf("Failed to destroy queue: %d", exit_code);

  getchar();

  return 0;
}