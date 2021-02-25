#include "buffer.h"

int buff_init(cycle_buff_t *const buffer) {
    if (!buffer) {
        return -1;
    }
    memset(buffer, 0, sizeof(cycle_buff_t));

    return 0;
}

int buff_write(cycle_buff_t *const buffer, const char c) {
    if (!buffer) {
        return -1;
    }
    buffer->data[buffer->wpos++] = c;
    buffer->wpos %= N;

    return 0;
}

int buff_read(cycle_buff_t *const buffer, char *const dst) {
    if (!buffer) {
        return -1;
    }
    *dst = buffer->data[buffer->rpos++];
    buffer->rpos %= N;

    return 0;
}
