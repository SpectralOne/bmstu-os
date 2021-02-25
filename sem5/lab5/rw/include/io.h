#ifndef __IO_H__
#define __IO_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <time.h>
#include <unistd.h>

#define ITERATIONS 20
#define WRITERS_COUNT 3
#define READERS_COUNT 5

#define MAX_SEMS 4
#define READER 0
#define WRITER 1

#define READ_QUEUE 2
#define WRITE_QUEUE 3

#define MAX_RANDOM 3

int reader_run(int *const shared_counter, const int sid, const int reader_id);
int writer_run(int *const shared_counter, const int sid, const int writer_id);

#endif // __IO_H__
