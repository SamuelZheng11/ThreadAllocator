#include "dispatchQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t mutex;

// singly linked list
typedef struct {
    queue_type_t queueType;
    struct node* next;
} sllNode;

dispatch_queue_t *dispatch_queue_create(queue_type_t queueType) {
    dispatch_queue_t queue;
    queue.queue_type = queueType;
}

void dispatch_queue_destroy(dispatch_queue_t *queue) {
    return free(queue);
}

task_t* task_create(void (* work)(void *), void *param, char* name) {
    task_t* task = (task_t*) malloc(sizeof(task_t));
    *task->name = *name;
    task->work = work;
    task->params = param;
    task->type = NULL;
    return &task;
}

void dispatch_async(dispatch_queue_t *queue, task_t *task) {
    // create a semaphore
    sem_init(&mutex, 0, 0);

    switch(queue->queue_type){
        case CONCURRENT:
            dispatch_concurrent_async(task);

        case SERIAL:
            dispatch_serial_async( task);

        // If the dispatch type is neither concurrent nor serial then exit the program with failure status
        default:
            exit(EXIT_FAILURE);
    }
}

void dispatch_concurrent_async(task_t *task) {
    
}

void dispatch_serial_async(task_t *task) {

}

int owl(int num)
{
    return num+1;
}

typedef struct {
    int x;
    int y;
} Coordinate;

int main(void)
{
    int a = 5;
    int (*the_func_pointer_name)(int);
    the_func_pointer_name = owl;
    int b = the_func_pointer_name(5);
    printf("%d\n", b);
    int* x = &b;
    *x = 2;
    x = &a;
    Coordinate* d = (Coordinate*) malloc(sizeof(Coordinate));
    d->x = 4;
    d->y = NULL;
}