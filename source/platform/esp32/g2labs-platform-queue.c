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
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/queue.h"

#define PLATFORM_QUEUE_TICKS_TO_WAIT (10)

typedef struct platform_queue {
    QueueHandle_t handle;
} platform_queue_t;

platform_queue_t* platform_queue_create(size_t element_size, size_t element_count) {
    platform_queue_t* q = calloc(1, sizeof(platform_queue_t));
    if (q) {
        q->handle = xQueueCreate(element_count, element_size);
    }
    return q;
}

void platform_queue_destroy(platform_queue_t* queue) {
    if (queue) {
        vQueueDelete(queue->handle);
        free(queue);
    }
}

static bool is_in_isr_context(void) {
    return (xPortInIsrContext() == pdTRUE);
}

bool platform_queue_push(platform_queue_t* queue, const void* element) {
    if (!queue) {
        return false;
    }
    BaseType_t status = pdFALSE;
    if (is_in_isr_context()) {
        BaseType_t _dummy;
        status = xQueueSendFromISR(queue->handle, element, &_dummy);
    } else {
        status = xQueueSend(queue->handle, element, PLATFORM_QUEUE_TICKS_TO_WAIT);
    }
    return (status == pdTRUE);
}

bool platform_queue_fetch(platform_queue_t* queue, void* element) {
    if (!queue) {
        return false;
    }
    BaseType_t status = pdFALSE;
    if (is_in_isr_context()) {
        BaseType_t _dummy;
        status = xQueueReceiveFromISR(queue->handle, element, &_dummy);
    } else {
        status = xQueueReceive(queue->handle, element, PLATFORM_QUEUE_TICKS_TO_WAIT);
    }
    return (status == pdTRUE);
}