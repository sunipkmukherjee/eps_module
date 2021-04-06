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
#include "eps_p31u/p31u.h"

#ifndef EPS_H
extern __thread pthread_cond_t eps_cmd_wait[1];
extern __thread pthread_mutex_t eps_cmd_wait_m[1];
#endif

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
int eps_get_hk(hkparam_t *hk);

/**
  * @brief
  *
  *
  * @param 
  * @return
  */
int eps_get_hk_out(eps_hk_out_t *hk_out);

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
int eps_lup_set(eps_lup_idx lup, int pw);
/**
 * @brief 
 * 
 * @param conf 
 * @return int 
 */
int eps_get_conf(eps_config_t *conf);
/**
 * @brief 
 * 
 * @param conf 
 * @return int 
 */
int eps_set_conf(eps_config_t *conf);

/**
  * @brief
  *
  *
  * @param 
  * @return
  */
int eps_hardreset();

#endif // EPS_EXTERN_H