#include "dispatchQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/sysinfo.h>
#include <unistd.h>

static int numberOfTasks;

// implementation of a linked list enqueue function
void push_dispatch_queue(dispatch_queue_t *queue, task_t task) {
    sll_node *currentNode = queue->nodeHead;
    sll_node *newNode = (sll_node*) malloc(sizeof(sll_node));
    newNode->task = task;
    newNode->next = NULL;

    while(currentNode->next != NULL) {
        currentNode = currentNode->next;
    }
    currentNode->next = newNode;
}

// implementation of a linked list dequeue function
sll_node *pop_dispatch_queue(dispatch_queue_t *queue) {
    sll_node *previousHead = queue->nodeHead;
    queue->nodeHead = previousHead->next;
    return previousHead;
}

// used to push a head onto the queue if it at any point is null
void push_head_on_queue(dispatch_queue_t *queue, task_t *task){
    sll_node *head = (sll_node*) malloc(sizeof(sll_node));
    head->next = NULL;
    head->task = *task;
    queue->nodeHead = head;
}

void generate_threads(dispatch_queue_t *queue) {
    // generate threads and execute the tasks on the semaphore queue
    int i;
    for(i = 0; i < get_nprocs_conf(); i++){
        pthread_t *thread = (pthread_t *) malloc(sizeof(pthread_t));
        pthread_create(thread , NULL, pollSemaphore, queue);
    }
}

// method that each thread calls when they run
void *pollSemaphore(void *param){
    // case to dispatch queue to prevent compiler warnings
    dispatch_queue_t *queue = (dispatch_queue_t*) param;
    sem_t lsem = queue->semaphore;
    // poll indefinatly
    while(1){
        sem_wait(&queue->semaphore);
        printf("executing task\n");
        sll_node *targetNode = pop_dispatch_queue(queue);
        targetNode->task.work;
    }
}

dispatch_queue_t *dispatch_queue_create(queue_type_t queueType) {
    // delearing variables to put in the structure
    sem_t semaphore;
    pthread_mutex_t mutex;
    dispatch_queue_t *queue = (dispatch_queue_t *) malloc(sizeof(dispatch_queue_t));

    // putting them into the struct
    queue->queue_type = queueType;
    queue->semaphore = semaphore;
    queue->mutex = mutex;

    // create a semaphore & mutex lock
    sem_init(&semaphore, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    return queue;
}

void dispatch_queue_destroy(dispatch_queue_t *queue) {
    free(queue);
    sem_destroy(&queue->semaphore);
    pthread_mutex_destroy(&queue->mutex);
}

task_t *task_create(void (* work)(void *), void *param, char* name) {
    // allocate memeory for the tast and populate the structure
    task_t *task = (task_t *) malloc(sizeof(task_t));
    *task->name = *name;
    task->work = work;
    task->params = param;
    // This is never used but for best practice a default of ASYNC is assigned
    task->type = ASYNC;

    return task;
}

void dispatch_concurrent_async(dispatch_queue_t *queue, task_t *task) {
    pthread_mutex_lock(&queue->mutex);
    push_dispatch_queue(queue, *task);
    pthread_mutex_unlock(&queue->mutex);
    sem_post(&queue->semaphore);
}

void dispatch_serial_async(dispatch_queue_t *queue, task_t *task) {
    
}

int dispatch_sync(dispatch_queue_t *queue, task_t *task){
    // check if the head of the linked list is assigned, if not assign the incoming task as the head
    if(queue->nodeHead == NULL) {
        push_head_on_queue(queue, task);
    }

    // perform sequential execution
    pthread_mutex_lock(&queue->mutex);
    push_dispatch_queue(queue, *task);
    pthread_mutex_unlock(&queue->mutex);
    sem_post(&queue->semaphore);
}

// the header specifies a int as the return value so that is what is done here, even thought the integer return value is never used
int dispatch_async(dispatch_queue_t *queue, task_t *task) {

    // check if the head of the linked list is assigned, if not assign the incoming task as the head
    if(queue->nodeHead == NULL) {
        push_head_on_queue(queue, task);
    }

    generate_threads(queue);

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
