/**
 * @file eps_iface.h
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief
 * @version 0.2
 * @date 2021-03-17
 *
 * @copyright Copyright (c) 2021
 *
 */

#ifndef EPS_IFACE_H
#define EPS_IFACE_H

#include <pthread.h>

int eps_init();
void eps_destroy();
void* eps_thread(void*);

#endif // EPS_IFACE_H