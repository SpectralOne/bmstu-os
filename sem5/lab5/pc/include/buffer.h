#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <string.h>
#include <unistd.h>

#define N 24

typedef char data_t[N];

typedef struct {
    size_t rpos;
    size_t wpos;
    data_t data;
} cycle_buff_t;

int buff_init(cycle_buff_t *const buffer);
int buff_write(cycle_buff_t *const buffer, const char c);
int buff_read(cycle_buff_t *const buffer, char* const dst);

#endif  // __BUFFER_H__
