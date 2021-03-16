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
p31u* eps;

/**
 * @brief The command queue.
 *
 */
cmdq_t* globalCmdQ;

/**
 * @brief Boolean to determine if this is the first pass.
 *
 */
int firstRun = 1;

/**
 * @brief Condition variable for use with eps_mutex.
 *
 */
pthread_cond_t eps_ready;

/**
 * @brief Mutex to ensure secure data access.
 *
 */
pthread_mutex_t eps_mutex;

bool eps_cmdq_enqueue(uint8_t value[]) {
	// Allocate memory for a new node.
	node_t* newNode = malloc(sizeof(node_t));

	// Check is malloc was successful.
	if (newNode == NULL) {
		perror("Unable to allocate memory to enqueue command queue!");
			return false;
	}

	// Initialize the values of newNode.
	newNode->cmd = value;
	newNode->next = NULL;

	// If the queue->tail already exists, set it's next to the
	// new node. This adds our node to the end of the list.
	if (globalCmdQ->tail != NULL) {
		globalCmdQ->tail->next = newNode;
	}

	// Regardless if a tail existed or not, we now want to establish
	// our latest addition to the command queue as the tail item.
	globalCmdQ->tail = newNode;

	// If the queue->head doesn't already exist, set it to the
	// new node. This would only occur if the queue was previously
	// empty, making newNode simultaneously the head and the tail.
	if (globalCmdQ->head == NULL) {
		globalCmdQ->head = newNode;
	}

	// Successful.
	return true;
}

node_t* eps_cmdq_dequeue() {
	// Check if the queue is empty, and return an error if so.
	if (globalCmdQ->head == NULL) {
		perror("Cannot dequeue an empty command queue!");
		return NULL;
	}

	// Set the returnNode to the current command queue's head.
	node_t* returnNode = globalCmdQ->head;

	// Set the command queue's head to the next node, since we
	// are removing the read node.
	globalCmdQ->head = globalCmdQ->head->next;

	// If the head is now NULL because ...->next was NULL
	// (aka there was no next), we should be sure to set our
	// tail to NULL as well.
	if (globalCmdQ->head == NULL) {
		globalCmdQ->tail = NULL;
	}

	// Return the old head node that we've dequeued.
	return returnNode;
}

// Dequeues the top next command, executes it, and frees the memory.
int eps_cmdq_execute() {
	node_t* node = dequeue();

	if (node == NULL) {
		perror("Dequeueing error!");
		return -1;
	}

	switch (node->cmd[0]) {
	case 0: // ping
		eps_p31u_ping(eps);
		break;
	case 1: // reboot
		eps_p31u_reboot(eps);
		break;
	case 2: // toggle latch
		eps_p31u_tgl_lup(eps, node->cmd[1]);
		break;
	case 3: // set latch
		eps_p31u_lup_set(eps, node->cmd[1], node->cmd[2]);
		break;
	default: // error
		perror("Error: Default case reached in eps_cmdq_execute()!");
		return -1;
	}

	free(node);
	return 1;
}

void eps_cmdq_destroy() {
	while (globalCmdQ->head != NULL) {
		free(dequeue());
	}
	return;
}

// Adds the command to the command queue.
// Only publicly exposed functions.
// Eventually, these should return an address to a location
// in memory which contains info on the current status of
// the command request: Pending, Executed, Deleted.
void eps_ping() {
	uint8_t arr[] = { 0 };
	enqueue(arr);

	return;
}

void eps_reboot() {
	uint8_t arr[] = { 1 };
	enqueue(arr);

	return;
}

void eps_latchToggle(eps_lup_idx lup) {
	uint8_t arr[] = { 2, lup };
	enqueue(arr);

	return;
}

void eps_latchSet(eps_lup_idx lup, int pw) {
	uint8_t arr[] = { 3, lup, pw };
	enqueue(globalCmdQ, arr);

	return;
}

// Initializes the EPS and ping-tests it.
// int eps_onAwake(){}
int eps_init() {
	//#ifdef EPS_Init

	eps = (p31u*)malloc(sizeof(p31u));
	globalCmdQ = (cmdq_t*)malloc(sizeof(cmdq_t));

	// Initializes the EPS component while checking if successful.
	if (eps_p31u_init(eps, 1, 0x1b) <= 0) {
		perror("EPS initialization failed!");
		return -1;
	}

	// If we can't successfully ping the EPS then something has gone wrong.
	if (eps_p31u_ping(eps) < 0) {
		perror("EPS ping test failed!");
		return -2;
	}

	//#endif // EPS_Init
}

// Accepts the Thread ID from its caller (main.c)
// void *eps_onStart(void *tid){}
void *eps_thread(void *tid) {

	// 'done' is a global Boolean established in main.h and handled in main.c
	// It is false when this module is alive and true when this module is being
	// shut down.
	// while(moduleAlive){}
	while (!done) {
		if (firstRun) {
			printf("EPS: Waiting for release...\n");
			firstRun = 0;
		}

		pthread_cond_wait(&eps_ready, &eps_mutex);

		eps_cmdq_execute();
	}

	pthread_exit(NULL);
}

// Frees eps memory and destroys the EPS object.
// void eps_onShutdown(p31u* eps){}
void eps_destroy() {
	free(eps);
	eps_cmdq_destroy();
	free(globalCmdQ);
	eps_p31u_destroy(eps);
}
