/**
 * @file eps_extern.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Externally accessible EPS command handling functions.
 * @version 0.2
 * @date 2021-03-17
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef EPS_EXTERN_H
#define EPS_EXTERN_H

#include <pthread.h>
#include <stdarg.h>
#ifndef EPS_H
extern __thread pthread_cond_t eps_cmd_wait[1];
extern __thread pthread_mutex_t eps_cmd_wait_m[1];
#endif
#define EPS_CMD_PING 0
#define EPS_CMD_REBOOT 1
#define EPS_CMD_TLUP 2
#define EPS_CMD_SLUP 3
#define EPS_CMD_HARDRESET 4
#define EPS_CMD_GET_HK 5
#define EPS_CMD_GET_HK_OUT 6
/**
 * @brief Command type for enqueuing.
 *
 * nCmds:       Number of commands.
 * cmds:        Array of commands where each is a single integer, i.e. EPS_CMD_PING.
 * nCmdArgs:    Array of the number of command arguments per command.
 *              i.e. nCmdArgs[i] is the number of arguments for command cmds[i].
 * cmdArgs:     A 2-dimensional array of command arguments.
 *              i.e. cmdArgs[i][k] is the k'th argument of the command cmds[i].
 * 
 */
typedef struct Command {
    int nCmds;
    int* cmds;
    int* nCmdArgs; 
    int** cmdArgs; 
} command_t;

 /**
  * @brief Enters a command into the queue for execution.
  *
  * Takes an array to determine the command and its arguments. Valid entries:
  * 0            : Pings the EPS.
  * 1            : Reboots the EPS.
  * 2 arg1       : Toggle latch with ID of arg1 (0 through 5).
  * 3 arg1 arg2  : Set latch with ID of arg1 (0 through 5) to state arg2 (0 off, 1 on).
  *
  * @param value An array of uint8_t to determine the command and its arguments.
  * @return int Return value for i2c read/write
  */
int eps_cmdq_enqueue(command_t *);

int *get_hk_ptr();

int *get_hkout_ptr();

void show_hk();

void show_hkout();

#endif // EPS_EXTERN_H