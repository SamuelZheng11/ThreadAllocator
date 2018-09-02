#include "dispatchQueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/sysinfo.h>
#include <unistd.h>

// implementation of a linked list enqueue function
void push_dispatch_queue(dispatch_queue_t *queue, task_t task) {
    sll_node *current_node = queue->nodeHead;
    sll_node *newNode = (sll_node*) malloc(sizeof(sll_node));
    newNode->task = task;
    newNode->next = NULL;

    while(current_node->next != NULL) {
        current_node = current_node->next;
    }
    current_node->next = newNode;
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

// method that releases the queue, semaphore and mutex lock memory when called
void dispatch_queue_destroy(dispatch_queue_t *queue) {
    free(queue);
    sem_destroy(&queue->semaphore);
    pthread_mutex_destroy(&queue->mutex);
}

// method that releases the memeory of a task when executed
void destroy_task(task_t *task) {
    free(task);
}

// method that each thread calls when they run concurrently
void *perform_tasks_concurrently(void *param){
    // case to dispatch queue to prevent compiler warnings
    dispatch_queue_t *queue = (dispatch_queue_t*) param;
    // poll indefinatly
    while(1){
        sem_wait(&queue->semaphore);
        // update the queue's active thread counter and aquire mutex lock
        pthread_mutex_lock(&queue->mutex);
        queue->busy_threads = queue->busy_threads + 1;
        // dequeuing a task from the queue and release the lock
        sll_node *targetNode = pop_dispatch_queue(queue);
        pthread_mutex_unlock(&queue->mutex);
        // performing task with task params
        targetNode->task.work(targetNode->task.params);
        // release memory
        destroy_task(&targetNode->task);
        // decrement active thread on the queue as this thread is finished
        queue->busy_threads = queue->busy_threads - 1;
    }
}

// method that each thread calls when they run concurrently
void *perform_tasks_serially(void *param){
    // case to dispatch queue to prevent compiler warnings
    dispatch_queue_t *queue = (dispatch_queue_t*) param;
    // poll indefinatly
    while(1){
        sem_wait(&queue->semaphore);
        // update the queue's active thread counter and aquire mutex lock
        pthread_mutex_lock(&queue->mutex);
        queue->busy_threads = queue->busy_threads + 1;
        // dequeuing a task from the queue and release the lock
        sll_node *targetNode = pop_dispatch_queue(queue);
        // performing task with task params
        targetNode->task.work(targetNode->task.params);
        // release memory
        destroy_task(&targetNode->task);
        // decrement active thread on the queue as this thread is finished
        queue->busy_threads = queue->busy_threads - 1;
        pthread_mutex_unlock(&queue->mutex);
    }
}

void generate_threads(dispatch_queue_t *queue, queue_type_t queue_type) {
    // generate threads and execute the tasks on the semaphore queue
    int i;
    for(i = 0; i < get_nprocs_conf(); i++){
        pthread_t *thread = (pthread_t *) malloc(sizeof(pthread_t));
        switch(queue_type) {
        case CONCURRENT:
            pthread_create(thread , NULL, perform_tasks_concurrently, queue);
            break;

        case SERIAL:
            pthread_create(thread , NULL, perform_tasks_serially, queue);

        }
    }
}

dispatch_queue_t *dispatch_queue_create(queue_type_t queue_type) {
    // delearing variables to put in the structure
    sem_t semaphore;
    pthread_mutex_t mutex;
    dispatch_queue_t *queue = (dispatch_queue_t *) malloc(sizeof(dispatch_queue_t));
    // putting them into the struct
    queue->queue_type = queue_type;
    queue->semaphore = semaphore;
    queue->mutex = mutex;
    // create a semaphore & mutex lock
    sem_init(&queue->semaphore, 0, 0);
    pthread_mutex_init(&queue->mutex, NULL);

    //generate threads
    generate_threads(queue, queue_type);

    return queue;
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
}

int dispatch_sync(dispatch_queue_t *queue, task_t *task){
    return 0;
}

// the header specifies a int as the return value so that is what is done here, even thought the integer return value is never used
int dispatch_async(dispatch_queue_t *queue, task_t *task) {
    // lock queue
    pthread_mutex_lock(&queue->mutex);

    // check if the head of the linked list is assigned, if not assign the incoming task as the head of the queue
    if(queue->nodeHead == NULL) {
        push_head_on_queue(queue, task);
    } else {
        push_dispatch_queue(queue, *task);
    }
    
    // unlock queue
    pthread_mutex_unlock(&queue->mutex);
    
    // notify semaphore that new tasks avalible
    sem_post(&queue->semaphore);
    return 0;
}

int dispatch_queue_wait(dispatch_queue_t *queue) {
    while(queue->nodeHead != NULL || queue->busy_threads > 0){
        // block until there are no more active theads on the queue and there are no more tasks on the queue
    }
    return 0;
}

void dispatch_for(dispatch_queue_t *queue, long number, void (*work) (long)) {
    task_t *task;
    char id;
    char names[10][2];
    for(id = 0; id < number; id++){
        char *name = names[id - 'A'];
        name[0] = id; name[1] = '\0';
        task = task_create(work, (void *)name, name);
        dispatch_async(queue, task);
    }
}
