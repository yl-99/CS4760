#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include "config.h"

void slave();

int main(int argc, char *argv[])
{

    pid_t childpid = 0;
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
            fprintf(stderr, "proc %i \n", numOfSlaveProc);
            fprintf(stderr, "proc %i \n", termTime);
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

    execl("./slave", "./slave","22", NULL);
}