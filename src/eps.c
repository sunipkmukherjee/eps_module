/**
 * @file eps.c
 * @author Mit Bailey (mitbailey99@gmail.com)
 * @brief Implementation of EPS command handling functions.
 * @version 0.3
 * @date 2021-03-17
 *
 * @copyright Copyright (c) 2021
 *
 */

#include "eps_p31u/p31u.h"
#include "eps.h"
#include <main.h>
#include <stdint.h>
#include <stdbool.h>
#include <pthread.h>

/* Variable allocation for EPS */

/**
  * @brief The EPS object pointer.
  *
  */
static p31u *eps = NULL;

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

int eps_ping(){
    if(eps==NULL){
        return -1;
    }

    return eps_p31u_ping(eps);
}

int eps_reboot(){
    if(eps==NULL){
        return -1;
    }

    return eps_p31u_reboot(eps);
}

int eps_get_hk(hkparam_t* hk){
    if(eps==NULL){
        return -1;
    }

    return eps_p31u_get_hk(eps, hk);
}

int eps_get_hk_out(eps_hk_out_t* hk_out){
    if(eps==NULL){
        return -1;
    }

    return eps_p31u_get_hk_out(eps, hk_out);
}

int eps_tgl_lup(eps_lup_idx lup){
    if(eps==NULL){
        return -1;
    }

    return eps_p31u_tgl_lup(eps, lup);
}

int eps_lup_set(eps_lup_idx lup, int pw){
    if(eps==NULL){
        return -1;
    }

    return eps_p31u_lup_set(eps, eps_lup_idx lup, int pw);
}

int eps_hardreset(){
    if(eps==NULL){
        return -1;
    }

    return eps_p31u_hardreset(eps);
}

// Initializes the EPS and ping-tests it.
int eps_init()
{
    eps = (p31u *)malloc(sizeof(p31u));

    // Check if malloc was successful.
    if (eps == NULL) {
        return -1;
    }

    // Initializes the EPS component while checking if successful.
    if (eps_p31u_init(eps, 1, 0x1b) <= 0) {
        return -1;
    }

    // If we can't successfully ping the EPS then something has gone wrong.
    if (eps_p31u_ping(eps) < 0) {
        return -2;
    }
}

void *eps_thread(void *tid)
{
    while (!done)
    {
        // Reset the watch-dog timer.
        eps_reset_wdt(eps);
    }

    pthread_exit(NULL);
}

// Frees eps memory and destroys the EPS object.
void eps_destroy()
{
    // Destroy / free the eps.
    eps_p31u_destroy(eps);
    free(eps);
    eps = NULL;
}

////////////////////////
// eps_module testing //
////////////////////////

static hkparam_t hkdata[1];
static eps_hk_out_t hkout[1];

int *get_hk_ptr()
{
    return (int *)hkdata;
}

int *get_hkout_ptr()
{
    return (int *)hkout;
}

#ifndef EPS_UNIT_TEST
#include <stdio.h>

void print_hk(hkparam_t *hk)
{
    printf("Photovoltaic voltage (mV):");
    for (int i = 0; i < 3; i++)
        printf(" %u", (hk->pv[i]));
    printf("\nTotal photo current [mA]: %u", (hk->pc));
    printf("\nBattery voltage [mV]: %u", (hk->bv));
    printf("\nTotal system current [mA]: %u", (hk->sc));
    printf("\nTemp of boost converters and onboard batt (C):");
    for (int i = 0; i < 4; i++)
        printf(" %d", (hk->temp[i]));
    printf("\nExternal board batt (C):");
    for (int i = 0; i < 2; i++)
        printf(" %d", (hk->batt_temp[i]));
    printf("\nNumber of latchups:");
    for (int i = 0; i < 6; i++)
        printf(" %u", (hk->latchup[i]));
    printf("\nCause of last reset: 0x%02x", hk->reset);
    printf("\nNumber of reboots: %u", (hk->bootcount));
    printf("\nSoftware errors: %u", (hk->sw_errors));
    printf("\nPPT mode: %u", hk->ppt_mode);
    printf("\nChannel status: %u", hk->channel_status);
    printf("\n\n");
}

void print_hk_out(eps_hk_out_t *hk_out)
{
    printf("LUP currents: ");
    for (int i = 0; i < 6; i++)
        printf("%u ", (hk_out->curout[i]));
    printf("\nLUP status: ");
    for (int i = 0; i < 8; i++)
        printf("%01x ", (hk_out->output[i]));
    printf("\nLUP on delay [s]: ");
    for (int i = 0; i < 8; i++)
        printf("%u ", (hk_out->output_on_delta[i]));
    printf("\nLUP off delay [s]: ");
    for (int i = 0; i < 8; i++)
        printf("%u ", (hk_out->output_off_delta[i]));
    printf("\nNumber of LUPs: ");
    for (int i = 0; i < 6; i++)
        printf("%u ", (hk_out->latchup[i]));
    printf("\n\n");
}

void show_hk()
{
    print_hk(hkdata);
    fflush(stdout);
}

void show_hkout()
{
    print_hk_out(hkout);
    fflush(stdout);
}

#endif
