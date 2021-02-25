#ifndef __RUNNERS_H__
#define __RUNNERS_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <time.h>

#include "buffer.h"

#define ITERATIONS_AMOUNT 8

#define CONS_COUNT 3
#define PROD_COUNT 3

#define BIN_SEM 0
#define BUF_FULL 1
#define BUF_EMPTY 2

#define MAX_RANDOM_PROD 2
#define MAX_RANDOM_CONS 5

int run_producer(cycle_buff_t *const buffer, const int sid, const int prod_id);
int run_consumer(cycle_buff_t *const buffer, const int sid, const int cons_id);

#endif  // __RUNNERS_H__