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

// Put other defines here.

#define EPS_I2C_ADDR 0x7d        // Temporary designation (from eps_telem.h)
#define EPS_I2C_BUS "/dev/i2c-0" // I2C bus (from eps_telem.h)

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
 * @brief TBD.
 * 
 * TBD
 * 
 * @return NULL.
 */
void eps_destroy();


#endif // EPS_H