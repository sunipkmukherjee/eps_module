#include "eps_extern.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

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
    printf("\n");

    // The command input.
    int inCommand = -1;

    while (!done)
    {
        printf("Please enter a valid command...\n");
        printf("> ");

        inCommand = scanf(" %d", &inCommand);

        switch (inCommand)
        {
        case EPS_CMD_PING:
            eps_ping();

            break;
        case EPS_CMD_REBOOT:
            eps_reboot();

            break;
        case EPS_CMD_TLUP:
            int LUPID = -1;

            printf("LUP ID: ");
            scanf(" %d", &LUPID);

            eps_tgl_lup(LUPID);

            break;
        case EPS_CMD_SLUP:
            int LUPID = -1;
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
            
            eps_get_hk(get_hk_ptr());

            break;
        case EPS_CMD_GET_HK_OUT:

            eps_get_hk_out(get_hkout_ptr())

            break;
        default:
            printf("Error: Invalid input.");
            fflush(stdout);
            break;
        }
    }

    return 0;
}