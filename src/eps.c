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
static cmdq_t *commandQueue;

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

// Enqueue() is the only publicly accessible EPS module function, and allows an
// outside caller to send the EPS a command request.

// Enqueue() should have a single command_t structure passed to it, properly
// filled out by the caller.

// Execute will treat a multi-command as multiple, adjacent commands, and will
// wait to return until an array can be constructed of all return values for the
// multi-command.
int eps_cmdq_enqueue(command_t *cmdRequest)
{
    // Allocate memory for a new node.
    cmdnode_t *newNode = malloc(sizeof(cmdnode_t));

    // Check if malloc was successful.
    if (newNode == NULL)
    {
        perror("Unable to allocate memory to enqueue command queue!");
        return NULL;
    }

    // Initialize the values of newNode by setting ->cmd to our args array, 
    // and ->next to NULL.
    newNode->cmd = cmdRequest;
    newNode->next = NULL;

    // Allocate memory for a return value array.
    int* retval = (int*)malloc(newNode->cmd->nCmds * sizeof(int));

    // Check if malloc was successful.
    if (retval == NULL)
    {
        perror("Unable to allocate memory to return value array!");
        return NULL;
    }

    // Point newNode's retval pointer to the start of our retval array memory.
    newNode->retval = retval;

    // If the queue->tail already exists, set it's next to the
    // new node. This adds our node to the end of the list.
    if (commandQueue->tail != NULL)
    {
        commandQueue->tail->next = newNode;
    }

    // Regardless if a tail existed or not, we now want to establish
    // our latest addition to the command queue as the tail item.
    commandQueue->tail = newNode;

    // If the queue->head doesn't already exist, set it to the
    // new node. This would only occur if the queue was previously
    // empty, making newNode simultaneously the head and the tail.
    if (commandQueue->head == NULL)
    {
        commandQueue->head = newNode;
    }

    //// Enqueueing has completed. ////

    // Wait until the command has been executed, then return.
    static struct timespec twait = {.tv_sec = EPS_CMD_TIMEOUT, .tv_nsec = 0};
    newNode->wakeup = eps_cmd_wait;
    pthread_cond_timedwait(eps_cmd_wait, eps_cmd_wait_m, &twait);

    free(newNode);

    // Return pointer to where the return value / array is.
    return retval;
}

cmdnode_t *eps_cmdq_dequeue()
{
    // Check if the queue is empty, and return an error if so.
    if (commandQueue->head == NULL)
    {
        perror("Cannot dequeue an empty command queue!");
        return NULL;
    }

    // Set the returnNode to the current command queue's head.
    cmdnode_t *returnNode = commandQueue->head;

    // Set the command queue's head to the next node, since we
    // are removing the read node.
    commandQueue->head = commandQueue->head->next;

    // If the head is now NULL because ...->next was NULL
    // (aka there was no next), we should be sure to set our
    // tail to NULL as well.
    if (commandQueue->head == NULL)
    {
        commandQueue->tail = NULL;
    }

    // Return the old head node that we've dequeued.
    return returnNode;
}

// Dequeues the top next command, executes it, and frees the memory.
int eps_cmdq_execute()
{
    cmdnode_t *node = eps_cmdq_dequeue();

    if (node == NULL)
    {
        perror("Dequeueing error!");
        return -1;
    }

    int nCmds = node->cmd->nCmds;


    // i tracks the current command number
    int i = 0, retval = 1;

    // Find what command this node holds and execute it.
    // The while loop ensures that if there are multiple commands,
    // we successfully execute all.
    while(i < nCmds){
        switch(node->cmd->cmds[i]){
            case EPS_CMD_PING:
                node->retval[i]=eps_p31u_ping(eps);
                i++; // completed a command
                break;
            case EPS_CMD_REBOOT:
                node->retval[i]=eps_p31u_reboot(eps);
                i++; 
                break;
            case EPS_CMD_TLUP:
                node->retval[i]=eps_p31u_tgl_lup(eps, node->cmd->cmdArgs[i][0]);
                i++; 
                break;
            case EPS_CMD_SLUP:
                node->retval[i] = eps_p31u_lup_set(eps, node->cmd->cmdArgs[i][0], 
                                                node->cmd->cmdArgs[i][1]);
                i++; 
                break;
            default: // error
                perror("Error: Default case reached in eps_cmdq_execute()!");
                retval = -1;
        }
    }

    pthread_cond_signal(node->wakeup);
    return retval;
}

void eps_cmdq_destroy()
{
    while (commandQueue->head != NULL)
    {
        free(eps_cmdq_dequeue());
    }
    return;
}

// Initializes the EPS and ping-tests it.
// int eps_onAwake(){}
int eps_init()
{
    //#ifdef EPS_Init

    eps = (p31u *)malloc(sizeof(p31u));

    // Check if malloc was successful.
    if (eps == NULL)
    {
        perror("Unable to allocate memory for eps object!");
        return NULL;
    }

    commandQueue = (cmdq_t *)malloc(sizeof(cmdq_t));

    // Check if malloc was successful.
    if (commandQueue == NULL)
    {
        perror("Unable to allocate memory for command queue!");
        return NULL;
    }

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
    free(commandQueue);

    // Destroy / free the eps.
    eps_p31u_destroy(eps);
    free(eps);
}