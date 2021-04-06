#include "eps_extern.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

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

hkparam_t hk[1];
eps_hk_out_t hkout[1];

void print_hk(hkparam_t hk)
{
    printf("Photovoltaic voltage (mV):");
    for (int i = 0; i < 3; i++)
        printf(" %u", (hk.pv[i]));
    printf("\nTotal photo current [mA]: %u", (hk.pc));
    printf("\nBattery voltage [mV]: %u", (hk.bv));
    printf("\nTotal system current [mA]: %u", (hk.sc));
    printf("\nTemp of boost converters and onboard batt (C):");
    for (int i = 0; i < 4; i++)
        printf(" %d", (hk.temp[i]));
    printf("\nExternal board batt (C):");
    for (int i = 0; i < 2; i++)
        printf(" %d", (hk.batt_temp[i]));
    printf("\nNumber of latchups:");
    for (int i = 0; i < 6; i++)
        printf(" %u", (hk.latchup[i]));
    printf("\nCause of last reset: 0x%02x", hk.reset);
    printf("\nNumber of reboots: %u", (hk.bootcount));
    printf("\nSoftware errors: %u", (hk.sw_errors));
    printf("\nPPT mode: %u", hk.ppt_mode);
    printf("\nChannel status: %u", hk.channel_status);
    printf("\n\n");
}

void print_hk_out(eps_hk_out_t hk_out)
{
    printf("LUP currents: ");
    for (int i = 0; i < 6; i++)
        printf("%u ", (hk_out.curout[i]));
    printf("\nLUP status: ");
    for (int i = 0; i < 8; i++)
        printf("%01x ", (hk_out.output[i]));
    printf("\nLUP on delay [s]: ");
    for (int i = 0; i < 8; i++)
        printf("%u ", (hk_out.output_on_delta[i]));
    printf("\nLUP off delay [s]: ");
    for (int i = 0; i < 8; i++)
        printf("%u ", (hk_out.output_off_delta[i]));
    printf("\nNumber of LUPs: ");
    for (int i = 0; i < 6; i++)
        printf("%u ", (hk_out.latchup[i]));
    printf("\n\n");
}

void *eps_test(void *tid)
{
    // One-time printouts
    printf("Unit Test Program for EPS Module\n");
    printf("********************************\n");
    printf("\n");
    printf("Valid Commands\n");
    printf("0: Ping\n");
    printf("1: Reboot\n");
    printf("2: Toggle LUP\n");
    printf("3: Set LUP\n");
    printf("4: Hard Reset\n");
    printf("5: Get HK\n");
    printf("6: Get HK Out\n");
    printf("^D: Quit\n");
    printf("\n");

    // The command input.
    int inCommand = -1, LUPID = -1;

    while (!done)
    {
        printf("Please enter a valid command...\n");
        printf("> ");

        inCommand = getchar() - '0';
        if (inCommand <= EOF)
            break;
        switch (inCommand)
        {
        case EPS_CMD_PING:
            eps_ping();

            break;
        case EPS_CMD_REBOOT:
            eps_reboot();

            break;
        case EPS_CMD_TLUP:
            LUPID = -1;

            printf("LUP ID: ");
            scanf(" %d", &LUPID);

            eps_tgl_lup(LUPID);

            break;
        case EPS_CMD_SLUP:
            LUPID = -1;
            int LUPState = -1;

            printf("LUP ID: ");
            scanf(" %d", &LUPID);

            printf("On (1) / Off (0): ");
            scanf(" %d", &LUPState);

            eps_lup_set(LUPID, LUPState);

            break;
        case EPS_CMD_HARDRESET:
            eps_hardreset();

            break;
        case EPS_CMD_GET_HK:

            eps_get_hk(hk);
            print_hk(hk[0]);

            break;
        case EPS_CMD_GET_HK_OUT:

            eps_get_hk_out(hkout);
            print_hk_out(hkout[0]);

            break;
        default:
            printf("Error: Invalid input.");
            fflush(stdout);
            break;
        }
    }
    done = 1;
    return NULL;
}
