/*
 * MIT License
 *
 * Copyright (c) 2023 Grzegorz Grzęda
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "g2labs-platform-queue.h"
#include <errno.h>
#include <fcntl.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct platform_queue {
    mqd_t id;
    size_t message_size;
    size_t message_count;
} platform_queue_t;

static void generate_random_string(char* buffer) {
    sprintf(buffer, "/a%d", rand());
}

platform_queue_t* platform_queue_create(size_t element_size, size_t element_count) {
    platform_queue_t* q = calloc(1, sizeof(platform_queue_t));
    if (q) {
        struct mq_attr attr = {
            .mq_msgsize = element_size,
            .mq_maxmsg = element_count,
            .mq_curmsgs = 0,
            .mq_flags = 0,
        };
        char buffer[20] = {0};
        generate_random_string(buffer);
        q->id = mq_open(buffer, O_CREAT | O_RDWR | O_NONBLOCK, 0777, &attr);
        if (q->id < 0) {
            perror("platform_queue_create [mq_open]");
            free(q);
            q = NULL;
        } else {
            q->message_count = element_count;
            q->message_size = element_size;
        }
    }
    return q;
}

void platform_queue_destroy(platform_queue_t* queue) {
    if (queue) {
        mq_close(queue->id);
        free(queue);
    }
}

bool platform_queue_push(platform_queue_t* queue, const void* element) {
    if (!queue || !element) {
        return false;
    }
    int result = mq_send(queue->id, (const char*)element, queue->message_size, 0);
    if (result) {
        perror("platform_queue_push [mq_send]");
    }
    return (0 == result);
}

bool platform_queue_fetch(platform_queue_t* queue, void* element) {
    if (!queue || !element) {
        return false;
    }
    size_t fetched_size = mq_receive(queue->id, (char*)element, queue->message_size, NULL);
    return (queue->message_size == fetched_size);
}