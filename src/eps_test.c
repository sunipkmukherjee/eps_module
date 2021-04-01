#include "eps_extern.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>

void *eps_test(void *tid)
{
        // Command type initialization.
    command_t *commandRequest = (command_t *)malloc(sizeof(command_t));

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

        inCommand = getchar() - '0';

        switch (inCommand)
        {
        case EPS_CMD_PING:
            // Set the commandRequest values
            commandRequest->nCmds = 1;
            commandRequest->cmds = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->cmds[0] = inCommand;
            commandRequest->nCmdArgs = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->nCmdArgs[0] = 0;
            commandRequest->cmdArgs = (int **)malloc(commandRequest->nCmds * sizeof(int *));
            commandRequest->cmdArgs[0] = NULL;

            break;
        case EPS_CMD_REBOOT:
            // Set the commandRequest values
            commandRequest->nCmds = 1;
            commandRequest->cmds = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->cmds[0] = inCommand;
            commandRequest->nCmdArgs = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->nCmdArgs[0] = 0;
            commandRequest->cmdArgs = (int **)malloc(commandRequest->nCmds * sizeof(int *));
            commandRequest->cmdArgs[0] = NULL;

            break;
        case EPS_CMD_TLUP:
            // Set the commandRequest values
            commandRequest->nCmds = 1;
            commandRequest->cmds = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->cmds[0] = inCommand;
            commandRequest->nCmdArgs = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->nCmdArgs[0] = 1;
            commandRequest->cmdArgs = (int **)malloc(commandRequest->nCmds * sizeof(int *));
            for (int i = 0; i < commandRequest->nCmds; i++)
                commandRequest->cmdArgs[i] = (int*)malloc(commandRequest->nCmdArgs[i] * sizeof(int));
            // Grab command arguments.
            printf("Enter one argument for EPS_CMD_TLUP: ");
            scanf(" %d", &(commandRequest->cmdArgs[0][0]));

            break;
        case EPS_CMD_SLUP:
            // Set the commandRequest values
            commandRequest->nCmds = 1;
            commandRequest->cmds = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->cmds[0] = inCommand;
            commandRequest->nCmdArgs = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->nCmdArgs[0] = 2;
            commandRequest->cmdArgs = (int **)malloc(commandRequest->nCmds * sizeof(int *));
            for (int i = 0; i < commandRequest->nCmds; i++)
                commandRequest->cmdArgs[i] = (int*)malloc(commandRequest->nCmdArgs[i] * sizeof(int));

            // Grab command arguments
            printf("Enter LUP ID: ");
            scanf(" %d", &(commandRequest->cmdArgs[0][0]));
            printf("Enter LUD value: ");
            scanf(" %d", &(commandRequest->cmdArgs[0][1]));

            break;
        case EPS_CMD_HARDRESET:
            // Set the commandRequest values
            commandRequest->nCmds = 1;
            commandRequest->cmds = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->cmds[0] = inCommand;
            commandRequest->nCmdArgs = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->nCmdArgs[0] = 0;
            commandRequest->cmdArgs = (int **)malloc(commandRequest->nCmds * sizeof(int *));
            commandRequest->cmdArgs[0] = NULL;

            break;
        case EPS_CMD_GET_HK:
            // Set the commandRequest values
            commandRequest->nCmds = 1;
            commandRequest->cmds = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->cmds[0] = inCommand;
            commandRequest->nCmdArgs = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->nCmdArgs[0] = 1;
            commandRequest->cmdArgs = (int **)malloc(commandRequest->nCmds * sizeof(int *));
            commandRequest->cmdArgs[0] = (int *) get_hk_ptr();

            break;
        case EPS_CMD_GET_HK_OUT:
            // Set the commandRequest values
            commandRequest->nCmds = 1;
            commandRequest->cmds = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->cmds[0] = inCommand;
            commandRequest->nCmdArgs = (int *)malloc(commandRequest->nCmds * sizeof(int));
            commandRequest->nCmdArgs[0] = 1;
            commandRequest->cmdArgs = (int **)malloc(commandRequest->nCmds * sizeof(int *));
            commandRequest->cmdArgs[0] = (int *) get_hkout_ptr();

            break;
        default:
            printf("Error: Invalid input.");
            break;
        }

        // Send the command.
        int *returnValue = eps_cmdq_enqueue(commandRequest);
        switch (inCommand)
        {
            case EPS_CMD_GET_HK:
                show_hk();
                break;
            case EPS_CMD_GET_HK_OUT:
                show_hkout();
        }
    }
    free(commandRequest);
    return 0;
}