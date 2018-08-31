#include "dispatchQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

dispatch_queue_t *dispatch_queue_create(queue_type_t queueType) {
    dispatch_queue_t *queue;
    return dispatch_queue_create(CONCURRENT);
}

void dispatch_queue_destroy(dispatch_queue_t *queue) {
    dispatch_queue_t *queue;
    dispatch_queue_destroy(queue);
}