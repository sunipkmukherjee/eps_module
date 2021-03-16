/**
 * @file eps.c
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-03-13
 *  
 * @copyright Copyright (c) 2021
 *
 */

// TO-DO
// Set up the command queue.
// See eps_p31u for how a CMD looks.
// The CmdQueue should handle all incoming command execution requests.

#include <p31u.h>
#include <eps.h>
#include <main.h>

p31u* eps;

/* Possible commands include...
 *
 * 0
 * Ping
 * static inline int eps_p31u_ping(p31u *dev)
 * 
 * 1
 * Reboot
 * static inline int eps_p31u_reboot(p31u *dev)
 * 
 * 2
 * Toggle Latch
 * static inline int eps_p31u_tgl_lup(p31u *dev, eps_lup_idx lup)
 * 
 * 3
 * Set Latch
 * static inline int eps_p31u_lup_set(p31u *dev, eps_lup_idx lup, int pw)
 * 
 */

// A Node of the commandQueue
struct Node {
    uint8_t cmd[];
    struct Node* next;
}

// The commandQueue struct.
// This holds a linked list of all commands queued for execution by the EPS.
struct CmdQueue {
    struct Node* head;
    struct Node* tail;
    uint8_t size;
    const uint8_t maxSize = 255;
} commandQueue;

bool enqueue(commandQueue *queue, uint8_t value[]){
    // Allocate memory for a new node.
    struct Node* newNode = malloc(sizeof(struct Node));

    // Check is malloc was successful.
    if (newNode == NULL){
        perror("Unable to allocate memory to enqueue command queue!")
        return false;
    }

    // Initialize the values of newNode.
    newNode->value = value;
    newNode->next = NULL;

    // If the queue->tail already exists, set it's next to the
    // new node. This adds our node to the end of the list.
    if (queue->tail != NULL){
        queue->tail->next = newNode;
    }
    
    // Regardless if a tail existed or not, we now want to establish
    // our latest addition to the command queue as the tail item.
    queue->tail = newNode;

    // If the queue->head doesn't already exist, set it to the
    // new node. This would only occur if the queue was previously
    // empty, making newNode simultaneously the head and the tail.
    if (queue->head == NULL){
        queue->head = newNode;
    }

    // Successful.
    return true;
}

struct Node* dequeue(commandQueue *queue){
    // Check if the queue is empty, and return an error if so.
    if (queue-> == NULL){
        perror("Cannot dequeue an empty command queue!");
        return -1;
    }

    // Set the returnNode to the current command queue's head.
    struct Node* returnNode = queue->head;

    // Set the command queue's head to the next node, since we
    // are removing the read node.
    queue->head = queue->head->next;

    // If the head is now NULL because ...->next was NULL
    // (aka there was no next), we should be sure to set our
    // tail to NULL as well.
    if (queue->head == NULL){
        queue->tail = NULL;
    }

    // Return the old head node that we've dequeued.
    return returnNode;
}

// Im not sure if this is needed, we will see.
// It might just so the same thing as dequeue.
void executeTopCmd(){

}

// Adds the command to the command queue.
// Only publicly exposed functions.
// Eventually, these should return an address to a location
// in memory which contains info on the current status of
// the command request: Pending, Executed, Deleted.
void eps_ping(){
    uint8_t arr[] = {0};
    enqueue(commandQueue, arr);

    return;
}

void eps_reboot(){
    uint8_t arr[] = {1};
    enqueue(commandQueue, arr);

    return;
}

void eps_latchToggle(uint8_t arg2){
    uint8_t arr[] = {2, arg2};
    enqueue(commandQueue, arr);

    return;
}

void eps_latchSet(uint8_t arg2, uint8_t arg3){
    uint8_t arr[] = {3, arg2, arg3};
    enqueue(commandQueue, arr);

    return;
}

// Initializes the EPS and ping-tests it.
// int eps_onAwake(){}
int eps_init(){
//#ifdef EPS_Init

    eps = (p31u*)malloc(sizeof(p31u));
    commandQueue = (struct CmdQueue*)malloc(sizeof(struct CmdQueue));

    // Initializes the EPS component while checking if successful.
    if(eps_p31u_init(eps, 1, 0x1b) <= 0){
        perror("EPS initialization failed!");
        return -1;
    }

    // If we can't successfully ping the EPS then something has gone wrong.
    if(eps_p31u_ping(eps) < 0){
        perror("EPS ping test failed!");
        return -2;
    }

//#endif // EPS_Init
}

// Accepts the Thread ID from its caller (main.c)
// void *eps_onStart(void *tid){}
void *eps_thread(void *tid){

    // 'done' is a global Boolean established in main.h and handled in main.c
    // It is false when this module is alive and true when this module is being
    // shut down.
    // while(moduleAlive){}
    while(!done){

    }

    pthread_exit(NULL);
}

// Frees eps memory and destroys the EPS object.
// void eps_onShutdown(p31u* eps){}
void eps_destroy(p31u* eps){
    free(eps);
    free(commandQueue);
    eps_p31u_destroy(eps);
}
