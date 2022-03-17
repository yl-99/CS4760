
// Program by: YOUSEF LANGI 3/17/22

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
#include <sys/sem.h>
#include "config.h"

extern int errno;

pid_t childpid;
int currentProc;
bool alarmFlag = false;
int semid;

union semun2
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    unsigned short *array; /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
} arg;

void alarm_handler(int num) // auto termination detection
{
    alarmFlag = true;

    write(STDOUT_FILENO, "Temnating proccess", 22);

    // rem semaphore
    if (semctl(semid, 0, IPC_RMID, arg) == -1)
    {
        perror("semctl");
        exit(1);
    }

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

    // rem semaphore
    if (semctl(semid, 0, IPC_RMID, arg) == -1)
    {
        perror("semctl");
        exit(1);
    }

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
    int numOfSlaveProc, termTime;

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
                fprintf(stderr, "%s: ", argv[0]);
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
                fprintf(stderr, "%s: ", argv[0]);
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

        fprintf(stderr, "%s: ", argv[0]);
        perror("Error");
        return 1;
    }

    // semaphore init
    key_t key;

    if ((key = ftok("master.c", 0)) == -1)
    {
        fprintf(stderr, "%s: ", argv[0]);
        perror("ftok Error");
        exit(1);
    }

    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1)
    {
        fprintf(stderr, "%s: ", argv[0]);
        perror("semget Error");
        exit(1);
    }

    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1)
    {
        fprintf(stderr, "%s: ", argv[0]);
        perror("semctl Error");
        exit(1);
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

    // rem semaphore
    if (semctl(semid, 0, IPC_RMID, arg) == -1)
    {
        fprintf(stderr, "%s: ", argv[0]);
        perror("semctl Error");
        exit(1);
    }

    fprintf(stderr, "Proccesses done\n");
}