#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include<string.h>
#include "config.h"
#include "shared_memory.h"

void slave();

int main(int argc, char *argv[])
{

    int numOfSlaveProc, termTime = 100;

    if (argc <= 1) // no input detected from cmd line
    {
        fprintf(stderr, "%s: ", argv[0] += 2);
        perror("Error");
        fprintf(stderr, "Try ./simplechain -h for help\n");
        return 1;
    }

    int option;
    while ((option = getopt(argc, argv, "t:h")) != -1)
    {
        switch (option)
        {

        case 'h':
            fprintf(stderr, "Usage: %s [-h] [-t maxTime numSlaveprocs] \n", argv[0]);
            return 1;
            break;

        case 't':
            if (argv[2] != NULL && atoi(argv[2]) != 0)
            {
                numOfSlaveProc = atoi(argv[2]);
            }
            else
            {
                perror("Error");
                return 1;
            }

            if (argv[3] != NULL && atoi(argv[3]) != 0)
            {
                termTime = atoi(argv[3]);
            }
            else
            {
                perror("Error");
                return 1;
            }

            if (numOfSlaveProc > MAXSLAVEPROC)
            {
                fprintf(stderr, "Number of slave processes exceed the allowed about. Slave processes reduced to maximum amount.\n");
                numOfSlaveProc = MAXSLAVEPROC;
            }
            fprintf(stderr, "slaveproc %i \n", numOfSlaveProc);
            fprintf(stderr, "termtime %i \n", termTime);
            break;

        default:
            // never enters
            return 1;
            break;
        }
    }

    if (optarg == NULL || atoi(optarg) == 0) // if delimiter '-' is used without option in cmd line
    {

        perror("Error");
        return 1;
    }

    char *memBlock = attachMemoryBlock(FILENAME, BLOCKSIZE); // memory init
    if (memBlock == NULL)
    {
        fprintf(stderr, "error, no block\n");
        return 1;
    }
    else
    {
        fprintf(stderr, "block OK\n");
        bool choosing[numOfSlaveProc];
        int shmidNum = shmget();
        int number = (int)shmat(shmid_num, NULL, 0);
        
        shmdt(memBlock); //detaches from shmem block
    }

    FILE *fptr;
    pid_t childpids[numOfSlaveProc], wpid;
    int status = 0;

    for (int i = 0; i < numOfSlaveProc; i++)
    {
        if ((childpids[i] = fork()) < 0)
        {
            break;
        }
        else if (childpids[i] == 0) // execution of slave process
        {
            char logFile[10];
            snprintf(logFile, sizeof(char) * 10, "logfile.%i", i); //creating log file
            fptr = fopen(logFile, "w");

            char slaveProcNumStr[5];
            sprintf(slaveProcNumStr, "%d", i);

            execl("./slave", "./slave", slaveProcNumStr, NULL); //executing slave

            fclose(fptr);
        }
    }
    while ((wpid = wait(&status)) > 0); // waits for all process to finish

    if (destroyMemoryBlock(FILENAME))
    {
        fprintf(stderr, "destroyed %s\n", FILENAME);
    }
    else
    {
        fprintf(stderr, "no destroyed %s\n", FILENAME);
    }
}