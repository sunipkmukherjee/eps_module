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
extern __thread pthread_cond_t *eps_cmd_wait;
extern __thread pthread_mutex_t *eps_cmd_wait_m;
#ifndef EPS_IFACE_H

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
void* eps_cmdq_enqueue(uint8_t value[]);

#endif // EPS_IFACE_H

#endif // EPS_EXTERN_H