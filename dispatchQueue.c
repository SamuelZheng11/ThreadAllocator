#include "dispatchQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/sysinfo.h>
#include <unistd.h>

sem_t semaphore;
pthread_mutex_t mutex;
static int numberOfTasks;

// implementation of a linked list enqueue function
void pushSllNode(dispatch_queue_t *queue, task_t task) {
    sllNode *currentNode = queue->nodeHead;
    sllNode *newNode = (sllNode*) malloc(sizeof(sllNode));
    newNode->task = task;
    newNode->next = NULL;

    while(currentNode->next != NULL) {
        currentNode = currentNode->next;
    }
    queue->nodeHead->next = newNode;
}

// implementation of a linked list dequeue function
void popSllNode(dispatch_queue_t *queue) {
    sllNode *previousHead = queue->nodeHead;
    queue->nodeHead = queue->nodeHead->next;
    free(previousHead);
}



void *pollSemaphore(void *param){
    // case to dispatch queue to prevent compiler warnings
    dispatch_queue_t *queue = (dispatch_queue_t*) param;
    // poll indefinatly
    while(1){
        sem_wait(&semaphore);
        queue->nodeHead->task.work;
    }
}

dispatch_queue_t *dispatch_queue_create(queue_type_t queueType) {
    dispatch_queue_t *queue = (dispatch_queue_t *) malloc(sizeof(dispatch_queue_t));
    queue->queue_type = queueType;

    // create a semaphore
    sem_init(&semaphore, 0, 0);

    // generate threads and execute the tasks on the semaphore queue
    int i;
    for(i = 0; i < get_nprocs_conf(); i++){
        pthread_t *thread = (pthread_t *) malloc(sizeof(pthread_t));
        pthread_create(thread , NULL, pollSemaphore, queue);
    }
    return queue;
}

void dispatch_queue_destroy(dispatch_queue_t *queue) {
    free(queue);
    sem_destroy(&semaphore);
    pthread_mutex_destroy(&mutex);
}

task_t *task_create(void (* work)(void *), void *param, char* name) {
    task_t *task = (task_t *) malloc(sizeof(task_t));
    *task->name = *name;
    task->work = work;
    task->params = param;
    // This is never used but for best practice a default of ASYNC is assigned
    task->type = ASYNC;
    return task;
}

void dispatch_concurrent_async(dispatch_queue_t *queue, task_t *task) {
    pthread_mutex_lock(&mutex);
    pushSllNode(queue, *task);
    pthread_mutex_unlock(&mutex);
    sem_post(&semaphore);
}

void dispatch_serial_async(dispatch_queue_t *queue, task_t *task) {

}

int dispatch_sync(dispatch_queue_t *queue, task_t *task){
    pthread_mutex_lock(&mutex);
    pushSllNode(queue, *task);
    pthread_mutex_unlock(&mutex);
    sem_post(&semaphore);
}

// the header specifies a int as the return value so that is what is done here, even thought the integer return value is never used
int dispatch_async(dispatch_queue_t *queue, task_t *task) {
    // break statements needed otherwise C will execute all enum cases (ie both concurrent and serial are executed)
    switch(queue->queue_type){
    case CONCURRENT:
        dispatch_concurrent_async(queue, task);
        break;

    case SERIAL:
        dispatch_serial_async(queue, task);
        break;

    // If the dispatch type is neither concurrent nor serial then exit the program with failure status
    default:
        exit(EXIT_FAILURE);
    }

    return 0;
}

int dispatch_queue_wait(dispatch_queue_t *queue) {

}

void dispatch_for(dispatch_queue_t *queue, long number, void (*work) (long)) {

}

// int owl(int num)
// {
//     return num+1;
// }

// typedef struct {
//     int x;
//     int y;
// } Coordinate;

// int main(void)
// {
//     int a = 5;
//     int (*the_func_pointer_name)(int);
//     the_func_pointer_name = owl;
//     int b = the_func_pointer_name(5);
//     printf("%d\n", b);
//     int* x = &b;
//     *x = 2;
//     x = &a;
//     Coordinate* d = (Coordinate*) malloc(sizeof(Coordinate));
//     d->x = 4;
//     d->y = NULL;
// }