/**
 * @file eps_extern.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Externally accessible EPS command handling functions.
 * @version 0.3
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

/*
Commands available externally:
Ping
Reboot
Get HK
Get HK Out
Toggle LUP
Set LUP
Hard Reset
*/

#define EPS_CMD_PING 0
#define EPS_CMD_REBOOT 1
#define EPS_CMD_TLUP 2
#define EPS_CMD_SLUP 3
#define EPS_CMD_HARDRESET 4
#define EPS_CMD_GET_HK 5
#define EPS_CMD_GET_HK_OUT 6

 /**
  * @brief
  *
  *
  * @param 
  * @return
  */
int eps_ping();

 /**
  * @brief
  *
  *
  * @param 
  * @return
  */
int eps_reboot();

 /**
  * @brief
  *
  *
  * @param 
  * @return
  */
int eps_get_hk(hkparam_t* hk);

 /**
  * @brief
  *
  *
  * @param 
  * @return
  */
int eps_get_hk_out(eps_hk_out_t* hk_out);

 /**
  * @brief
  *
  *
  * @param 
  * @return
  */
int eps_tgl_lup(eps_lup_idx lup);

 /**
  * @brief
  *
  *
  * @param 
  * @return
  */
int eps_lup_set(eos_lup_idx lup, int pw);

 /**
  * @brief
  *
  *
  * @param 
  * @return
  */
int eps_hardreset();

int *get_hk_ptr();

int *get_hkout_ptr();

void show_hk();

void show_hkout();

#endif // EPS_EXTERN_H