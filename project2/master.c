
// Program by: YOUSEF LANGI 2/24/22

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include "config.h"

extern int errno;

pid_t childpid;
bool *choosing;
int shmid_choosing;
int *number;
int shmid_number;
int currentProc;
bool alarmFlag = false;

void alarm_handler(int num) // auto termination detection
{
    alarmFlag = true;

    write(STDOUT_FILENO, "Terminating proccess", 22);

    // removing shmem
    shmctl(shmid_choosing, IPC_RMID, NULL);
    shmctl(shmid_number, IPC_RMID, NULL);

    char logFile[20];
    snprintf(logFile, sizeof(char) * 20, "logfile.%i", currentProc); // creating logfile.xx
    FILE *fptr;
    fptr = fopen(logFile, "a");

    struct tm *time_info;
    time_t current_time;
    char timeString[9]; // space for "HH:MM:SS\0"
    time(&current_time);
    time_info = localtime(&current_time); // time info for logfile.xx
    strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
    fprintf(fptr, "%s Child process %i is killed\n", timeString, currentProc);

    fclose(fptr);

    kill(childpid, SIGTERM);
    
}

void cntrlC_handler(int num) // ctrl^C detection
{
    alarmFlag = true;

    write(STDOUT_FILENO, "\nInput key dectected: terminating proccess\n", 50);

    // removing shmem
    shmctl(shmid_choosing, IPC_RMID, NULL);
    shmctl(shmid_number, IPC_RMID, NULL);

    char logFile[20];
    snprintf(logFile, sizeof(char) * 20, "logfile.%i", currentProc); // creating logfile.xx
    FILE *fptr;
    fptr = fopen(logFile, "a");

    struct tm *time_info;
    time_t current_time;
    char timeString[9]; // space for "HH:MM:SS\0"
    time(&current_time);
    time_info = localtime(&current_time); // time info for logfile.xx
    strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);
    fprintf(fptr, "%s Child process %i is killed\n", timeString, currentProc);

    fclose(fptr);

    kill(childpid, SIGTERM);
}

int main(int argc, char *argv[])
{
    pid_t childpid;
    int numOfSlaveProc, termTime;
    int status = 0;

    FILE *fptr;
    struct tm *time_info;
    time_t current_time;
    char timeString[9]; // space for "HH:MM:SS\0"

    if (argc <= 1) // no input detected from cmd line
    {
        fprintf(stderr, "%s: ", argv[0] += 2);
        perror("Error");
        fprintf(stderr, "Try ./master -h for help\n");
        return 1;
    }

    int option;
    while ((option = getopt(argc, argv, "t:h")) != -1)
    {
        switch (option)
        {

        case 'h':
            fprintf(stderr, "Usage: %s [-h] [-t numSlaveprocs maxTime] \n", argv[0]);
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
            else if (argv[3] == NULL)
            {
                termTime = TERMINATION_TIME;
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
            fprintf(stderr, "Slave Processes %i \n", numOfSlaveProc);
            fprintf(stderr, "Termination Time %i \n", termTime);
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
    shmdt(choosing); // detaching

    key = ftok("./master.c", 1);
    shmid_number = shmget(key, sizeof(number), 0644 | IPC_CREAT);
    number = shmat(shmid_number, NULL, 0);
    number[0] = 0;
    shmdt(number); // detaching

    if (shmid_number == -1) // incase of shmem error
    {
        fprintf(stderr, "%s", strerror(errno));
    }

    fprintf(stderr, "Running processes...\n");
    for (int i = 0; i < numOfSlaveProc; i++)
    {
        signal(SIGALRM, alarm_handler);
        signal(SIGINT, cntrlC_handler);

        childpid = fork();
        alarm(termTime); // terminatoin alarm

        if (childpid == 0) // execution of child process
        {
            currentProc = i;
            char slaveProcNumStr[5];
            sprintf(slaveProcNumStr, "%d", i);

            char logFile[20];
            snprintf(logFile, sizeof(char) * 20, "logfile.%i", i); // creating logfile.xx

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
        
    }

    fprintf(stderr, "Proccesses done\n");

    
    // removing shmem
    shmctl(shmid_choosing, IPC_RMID, NULL);
    shmctl(shmid_number, IPC_RMID, NULL);

    
}