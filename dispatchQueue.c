#include "dispatchQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct typedLinkedList {
    queue_type_t queueType;
    struct node * next;
} typedLinkedList_t;

dispatch_queue_t *dispatch_queue_create(queue_type_t queueType) {
    dispatch_queue_t queue;
    queue.queue_type = queueType;
}

void dispatch_queue_destroy(dispatch_queue_t *queue) {
    return free(*queue);
}

// ToDo
task_t *task_create(void (* work)(void *), void *param, char* name) {

}
