/**
 * @file eps.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Declarations of EPS command handling functions and structures.
 * @version 0.2
 * @date 2021-03-17
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef EPS_H
#define EPS_H

#include "eps_extern.h"
#include "eps_iface.h"
#include <stdbool.h>
#include <stdint.h>

#define EPS_CMD_TIMEOUT 5
#define EPS_LOOP_TIMER 1 // seconds


/**
 * @brief Node object for the command queue.
 *
 * cmd: Points to where the command array begins.
 * next: Points to the next CmdNode in the queue.
 * retval: Points to the location of the return value or array of values.
 *
 */
typedef struct CmdNode {
    command_t* cmd;
    pthread_cond_t *wakeup;
    struct CmdNode* next;
    int* retval;
} cmdnode_t;

/**
 * @brief Queue which holds EPS commands for processing.
 *
 */
typedef struct CmdQ {
    cmdnode_t* head;
    cmdnode_t* tail;
    uint8_t size;
    const uint8_t maxSize; // maxSize = 255
} cmdq_t;

/**
 * @brief Adds a command to the queue.
 *
 * @return Pointer to a location which will hold the return value on execution.
 */
int eps_cmdq_enqueue(command_t *);

/**
 * @brief Returns and removes the next command in the queue.
 *
 * @return note_t* The next command in the queue.
 */
cmdnode_t* eps_cmdq_dequeue();

/**
 * @brief Executes the next command in the queue.
 *
 * @return int 1 on success, -1 on dequeue error
 */
int eps_cmdq_execute();

/**
 * @brief Destroys / frees the entire command queue.
 *
 */
void eps_cmdq_destroy();

#endif // EPS_H