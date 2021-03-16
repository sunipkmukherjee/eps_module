/**
 * @file eps.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-03-13
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef EPS_H
#define EPS_H
#include <eps_extern.h>
#include <stdbool.h>

 /**
  * @brief Node object for the command queue.
  *
  */
typedef struct Node {
	uint8_t* cmd;
	node_t* next;
} node_t;

/**
 * @brief Queue which holds EPS commands for processing.
 *
 */
typedef struct CmdQ {
	node_t* head;
	node_t* tail;
	uint8_t size;
	const uint8_t maxSize; // maxSize = 255
} cmdq_t;

/**
 * @brief Adds a command to the queue.
 *
 * @return bool True on success.
 */
bool eps_cmdq_enqueue(uint8_t value[]);

/**
 * @brief Returns and removes the next command in the queue.
 *
 * @return note_t* The next command in the queue.
 */
node_t* eps_cmdq_dequeue();

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

/**
 * @brief Queues an EPS ping command request.
 *
 */
void eps_ping();

/**
 * @brief Queues an EPS reboot command request.
 *
 */
void eps_reboot();

/**
 * @brief Queues an EPS latch toggle command request.
 *
 * @param lup ID of latchup (0 -> 5V1, 1 -> 5V2, 2 -> 5V3, 3 -> 3V1, 4 -> 3V2, 5 -> 3V3)
 */
void eps_latchToggle(eps_lup_idx lup);

/**
 * @brief Queues an EPS latch set command request.
 *
 * @param lup ID of latchup (0 -> 5V1, 1 -> 5V2, 2 -> 5V3, 3 -> 3V1, 4 -> 3V2, 5 -> 3V3)
 * @param pw 0 for off, 1 for on
 */
void eps_latchSet(eps_lup_idx lup, int pw);

 /**
  * @brief Initializes the devices required to run the electronic power supply.
  *
  * This function initializes everything necessary for the EPS to operate.
  *
  * @return int 1 on success, error codes defined in SH_ERRORS on error.
  */
int eps_init();

/**
 * @brief EPS thread.
 *
 * @param tid Pointer to an integer containing the thread ID.
 * @return NULL.
 */
void *eps_thread(void *tid);

/**
 * @brief Frees and destroys.
 *
 *
 * @return NULL.
 */
void eps_destroy();


#endif // EPS_H