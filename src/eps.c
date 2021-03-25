/**
 * @file eps.c
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Implementation of EPS command handling functions.
 * @version 0.2
 * @date 2021-03-17
 *
 * @copyright Copyright (c) 2021
 *
 */

#include <p31u.h>
#include <eps.h>
#include <main.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

/* Variable allocation for EPS */

/**
  * @brief The EPS object pointer.
  *
  */
static p31u *eps;

/**
 * @brief The command queue.
 *
 */
static cmdq_t *globalCmdQ;

/**
 * @brief The return queue.
 *
 */
static retq_t *globalRetQ;

/**
 * @brief Thread local variable that enqueue waits upon
 * 
 */
__thread pthread_cond_t eps_cmd_wait[1];
/**
 * @brief Mutex for the queue wait
 * 
 */
__thread pthread_mutex_t eps_cmd_wait_m[1];

void *eps_cmdq_enqueue(uint8_t value[])
{
    // Check if the command ID, value[0], coordinates correctly with the size of
    // the array (number of arguments).
    // NOT YET IMPLEMENTED!

    int retval = 0;
    // Allocate memory for a new node.
    cmdnode_t *newNode = malloc(sizeof(cmdnode_t));

    // Check if malloc was successful.
    if (newNode == NULL)
    {
        perror("Unable to allocate memory to enqueue command queue!");
        return NULL;
    }

    // Initialize the values of newNode.
    newNode->cmd = value;
    newNode->next = NULL;

    // If the queue->tail already exists, set it's next to the
    // new node. This adds our node to the end of the list.
    if (globalCmdQ->tail != NULL)
    {
        globalCmdQ->tail->next = newNode;
    }

    // Regardless if a tail existed or not, we now want to establish
    // our latest addition to the command queue as the tail item.
    globalCmdQ->tail = newNode;

    // If the queue->head doesn't already exist, set it to the
    // new node. This would only occur if the queue was previously
    // empty, making newNode simultaneously the head and the tail.
    if (globalCmdQ->head == NULL)
    {
        globalCmdQ->head = newNode;
    }

    static struct timespec twait = {.tv_sec = EPS_CMD_TIMEOUT, .tv_nsec = 0};
    newNode->wakeup = eps_cmd_wait;
    pthread_cond_timedwait(eps_cmd_wait, eps_cmd_wait_m, &twait);

    retval = newNode->retval;

    free(newNode);
    // Return pointer to where the return value will be.
    return retval;
}

cmdnode_t *eps_cmdq_dequeue()
{
    // Check if the queue is empty, and return an error if so.
    if (globalCmdQ->head == NULL)
    {
        perror("Cannot dequeue an empty command queue!");
        return NULL;
    }

    // Set the returnNode to the current command queue's head.
    cmdnode_t *returnNode = globalCmdQ->head;

    // Set the command queue's head to the next node, since we
    // are removing the read node.
    globalCmdQ->head = globalCmdQ->head->next;

    // If the head is now NULL because ...->next was NULL
    // (aka there was no next), we should be sure to set our
    // tail to NULL as well.
    if (globalCmdQ->head == NULL)
    {
        globalCmdQ->tail = NULL;
    }

    // Return the old head node that we've dequeued.
    return returnNode;
}

// Dequeues the top next command, executes it, and frees the memory.
int eps_cmdq_execute()
{
    int retval = 1;
    cmdnode_t *node = eps_cmdq_dequeue();

    if (node == NULL)
    {
        perror("Dequeueing error!");
        return -1;
    }

    switch (node->cmd[0])
    {
    case 0: // ping
        node->retval = eps_p31u_ping(eps);
        break;
    case 1: // reboot
        node->retval = eps_p31u_reboot(eps);
        break;
    case 2: // toggle latch
        node->retval = eps_p31u_tgl_lup(eps, node->cmd[1]);
        break;
    case 3: // set latch
        node->retval = eps_p31u_lup_set(eps, node->cmd[1], node->cmd[2]);
        break;
    default: // error
        perror("Error: Default case reached in eps_cmdq_execute()!");
        retval = -1;
    }

    // The command has been executed and its return value has been placed
    // in memory at the corresponding globalReturnQueue->head->retval.
    // This particular return node can be removed and freed, and the
    // location in memory is still accessible by the calling function.
    // eps_retq_remove();

    // free(node);
    pthread_cond_signal(node->wakeup);
    return retval;
}

void eps_cmdq_destroy()
{
    while (globalCmdQ->head != NULL)
    {
        free(eps_cmdq_dequeue());
    }
    return;
}

retnode_t *eps_retq_allocate()
{
    // Allocate memory for a new node.
    retnode_t *newNode = malloc(sizeof(retnode_t));

    // Check if malloc was successful.
    if (newNode == NULL)
    {
        perror("Unable to allocate memory to enqueue return queue!");
        return NULL;
    }

    // Initialize the values of newNode.
    newNode->retval = NULL; // This will be set once the command executes.
    newNode->next = NULL;

    // If the queue->tail already exists, set it's next to the
    // new node. This adds our node to the end of the list.
    if (globalRetQ->tail != NULL)
    {
        globalRetQ->tail->next = newNode;
    }

    // Regardless if a tail existed or not, we now want to establish
    // our latest addition to the return queue as the tail item.
    globalRetQ->tail = newNode;

    // If the queue->head doesn't already exist, set it to the
    // new node. This would only occur if the queue was previously
    // empty, making newNode simultaneously the head and the tail.
    if (globalRetQ->head == NULL)
    {
        globalRetQ->head = newNode;
    }

    return newNode;
}

void *eps_retq_remove()
{
    // Check if the queue is empty, and return an error if so.
    if (globalRetQ->head == NULL)
    {
        perror("Cannot dequeue an empty return queue!");
        return NULL;
    }

    // Set the removeNode to the current return queue's head.
    retnode_t *removeNode = globalRetQ->head;
    void *retval = globalRetQ->head->retval;

    // Set the return queue's head to the next node, since we
    // are removing the read node.
    globalRetQ->head = globalRetQ->head->next;

    // If the head is now NULL because ...->next was NULL
    // (aka there was no next), we should be sure to set our
    // tail to NULL as well.
    if (globalRetQ->head == NULL)
    {
        globalRetQ->tail = NULL;
    }

    // Free the removed node.
    free(removeNode);

    // Return nothing.
    return retval;
}

void eps_retq_destroy()
{
    while (globalRetQ->head != NULL)
    {
        void *retval = eps_retq_remove();
        free(retval);
    }
}

// Initializes the EPS and ping-tests it.
// int eps_onAwake(){}
int eps_init()
{
    //#ifdef EPS_Init

    eps = (p31u *)malloc(sizeof(p31u));
    globalCmdQ = (cmdq_t *)malloc(sizeof(cmdq_t));

    // Initializes the EPS component while checking if successful.
    if (eps_p31u_init(eps, 1, 0x1b) <= 0)
    {
        perror("EPS initialization failed!");
        return -1;
    }

    // If we can't successfully ping the EPS then something has gone wrong.
    if (eps_p31u_ping(eps) < 0)
    {
        perror("EPS ping test failed!");
        return -2;
    }

    //#endif // EPS_Init
}

// Accepts the Thread ID from its caller (main.c)
// void *eps_onStart(void *tid){}
void *eps_thread(void *tid)
{

    // 'done' is a global Boolean established in main.h and handled in main.c
    // It is false when this module is alive and true when this module is being
    // shut down.
    // while(moduleAlive){}
    while (!done)
    {
        // Reset the watch-dog timer.
        eps_reset_wdt(eps);

        // Execute a command.
        eps_cmdq_execute();
    }

    pthread_exit(NULL);
}

// Frees eps memory and destroys the EPS object.
// void eps_onShutdown(p31u* eps){}
void eps_destroy()
{
    // Destroy / free command queue.
    eps_cmdq_destroy();
    free(globalCmdQ);

    // Destroy / free return queue.
    eps_retq_destroy();
    free(globalRetQ);

    // Destroy / free the eps.
    free(eps);
    eps_p31u_destroy(eps);
}