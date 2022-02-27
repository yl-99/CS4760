#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include "config.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

// #include "shared_memory.h"
pid_t childpid;
extern int errno;
  bool *choosing;
    int shmid_choosing;
    int *number;
    int shmid_number;

void alarm_handler(int num)
{
    write(STDOUT_FILENO, "Terminating proccess\n", 13);
            kill(childpid, 9);
            wait(NULL);
            shmctl(shmid_choosing, IPC_RMID, NULL);
    shmctl(shmid_number, IPC_RMID, NULL);
        
}
void cntrlC_handler(int num)
{
    write(STDOUT_FILENO, "\nProcess Killed\n", 13);
    kill(childpid,9);
    shmctl(shmid_choosing, IPC_RMID, NULL);
    shmctl(shmid_number, IPC_RMID, NULL);
}

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

  

    // shmem init
    key_t key;

    key = ftok("./master.c", 0);
    shmid_choosing = shmget(key, sizeof(choosing), 0644 | IPC_CREAT);
    choosing = shmat(shmid_choosing, NULL, 0);
    choosing[0] = false;
    shmdt(choosing);
    //fprintf(stderr, "%i\n", shmid_choosing);

    key = ftok("./master.c", 1);
    shmid_number = shmget(key, sizeof(number), 0644 | IPC_CREAT);
    choosing = shmat(shmid_choosing, NULL, 0);
    number = shmat(shmid_number, NULL, 0);
    number[0] = 0;
    shmdt(number);
  //  fprintf(stderr, "%i", shmid_choosing);

    if (shmid_number == -1)
    {
        fprintf(stderr, "%s", strerror(errno));
    }

    FILE *fptr;
    pid_t childpid;

    int status = 0;

    struct tm *time_info;
    time_t current_time;
    char timeString[9]; // space for "HH:MM:SS\0"

    signal(SIGALRM, alarm_handler);
    signal(SIGINT, cntrlC_handler);

    for (int i = 0; i < numOfSlaveProc; i++)
    {
        childpid = fork();
        alarm(termTime);

        if (childpid == 0) // execution of child process
        {
            char slaveProcNumStr[5];
            sprintf(slaveProcNumStr, "%d", i);

            char logFile[10];
            snprintf(logFile, sizeof(char) * 10, "logfile.%i", i); // creating logfile.xx

            fptr = fopen(logFile, "a");

            time(&current_time);
            time_info = localtime(&current_time); // time info for logfile.xx
            strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
            fprintf(fptr, "%s Child process %i is executing slave process\n", timeString, i);
            fclose(fptr);

            execl("./slave", "./slave", slaveProcNumStr, NULL); // executing slave process
            exit(0);
        }
    }
    for (int i = 0; i < numOfSlaveProc; i++) // waiting for child processes to finish
    {
        wait(NULL);
        fprintf(stderr, "proc %i dne\n", i);
    }

    fprintf(stderr, "done\n");
    shmctl(shmid_choosing, IPC_RMID, NULL);
    shmctl(shmid_number, IPC_RMID, NULL);
}