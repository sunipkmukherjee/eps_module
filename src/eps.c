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

// Enqueue() is the only publicly accessible EPS module function, and allows an
// outside caller to send the EPS a command request.
// The function is capable of taking a single command...
// (uint8_t nCmds, uint8_t nArgs, uint8_t cmd1, uint8_t cmd1Arg1)
// (1, 2, 2, 0)
// ...as well as multiple commands in the form of a multi-command...
// (5,    7,      0,     1,     0,     0,     3,   1,   0)
//  nCmds nArgs | cmd1 | cmd2 | cmd3 | cmd4 | cmd5 arg1 arg2 |
// Execute will treat a multi-command as multiple, adjacent commands, and will
// wait to return until an array can be constructed of all return values for the
// multi-command.
int* eps_cmdq_enqueue(uint8_t nCmds, uint8_t nArgs, ...)
{
    // Allocate memory for an array to store the arguments passed as var args.
    int* args[] = (int*)malloc(nArgs*sizeof(int));

        // Check if malloc was successful.
    if (args == NULL)
    {
        perror("Unable to allocate memory to arguments array!");
        return NULL;
    }

    // Begin access to passed var args
    va_list arguments;
    va_start(arguments. nArgs);

    // Set the first value of args to the total number of commands.
    args[0] = nCmds;

    // Transfer arguments from va_list arguments to our array of ints, args[]
    for(int i = 1; i <= nArgs; i++){
        args[i] = va_args(arguments, int);
    }

    // Conclude our var arg access.
    va_end(arguments);

    //// Enqueueing takes place below ////

    // Allocate memory for a new node.
    cmdnode_t *newNode = malloc(sizeof(cmdnode_t));

    // Check if malloc was successful.
    if (newNode == NULL)
    {
        perror("Unable to allocate memory to enqueue command queue!");
        return NULL;
    }

    // Allocate memory for a return value array.
    int* retval = (int*)malloc(nCmds * sizeof(int));

    // Check if malloc was successful.
    if (retval == NULL)
    {
        perror("Unable to allocate memory to return value array!");
        return NULL;
    }

    // Point newNode's retval pointer to the start of our retval array memory.
    newNode->retval = retval;

    // Initialize the values of newNode by setting ->cmd to our args array, 
    // and ->next to NULL.
    newNode->cmd = args;
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

    //// Enqueueing has completed. ////

    // Wait until the command has been executed, then return.
    static struct timespec twait = {.tv_sec = EPS_CMD_TIMEOUT, .tv_nsec = 0};
    newNode->wakeup = eps_cmd_wait;
    pthread_cond_timedwait(eps_cmd_wait, eps_cmd_wait_m, &twait);

    // Create a pointer to an integer or integer array and set it equal to
    // newNode's retval (which, itself, is a pointer) so we can free newNode.
    // int* retval = newNode->retval;
    // We don't need to do this anymore since we still have the original 
    // int pointer when we malloc'ed it.

    free(newNode);

    // Return pointer to where the return value / array is.
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
    nCmds = node->cmd[0];

    cmdnode_t *node = eps_cmdq_dequeue();

    if (node == NULL)
    {
        perror("Dequeueing error!");
        return -1;
    }

    // i tracks the current command number
    // k tracks the current arguments number
    // Note: cmd[0] is nCmds, not an actual command argument.
    int i = 0, k = 1;

    // Find what command this node holds and execute it.
    // The while loop ensures that if the command is a multi-command,
    // we successfully execute all commands in the multi-command.
    while(i<nCmds){
        switch(node->cmd[k]){
            case 0: // ping
                retval[i]=eps_p31u_ping(eps);
                i++; // completed a command
                k++; // ping has one argument
                break;
            case 1: // reboot
                retval[i]=eps_p31u_reboot(eps);
                i++; 
                k++;
                break;
            case 2: // latch toggle
                retval[i]=eps_p31u_tgl_lup(eps, node->cmd[k+1]);
                i++; 
                k+=2; // ltgl has 2 arguments
                break;
            case 3: // latch set
                retval[i] = eps_p31u_lup_set(eps, node->cmd[k+1], node->cmd[k+2]);
                i++; 
                k+=3; // lset has 3 arguments
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