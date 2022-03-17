
// Program by: YOUSEF LANGI 3/17/22

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/sem.h>
#include "config.h"

extern int errno;

int main(int argc, char *argv[])
{
    int procNum = atoi(argv[1]);

    srand(time(NULL));
    int sleepTime;
    struct tm *time_info;
    time_t current_time;
    char timeString[9]; // space for "HH:MM:SS\0"

    FILE *fptr;
    char logFile[20];
    snprintf(logFile, sizeof(char) * 20, "logfile.%i", procNum); // creating logfile.xx

    // receiving initialized semaphore
    key_t key;
    int semid;
    struct sembuf sb = {0, -1, 0};

    if ((key = ftok("master.c", 0)) == -1)
    {
        fprintf(stderr, "%s: ", argv[0]);
        perror("ftok Error");
        exit(1);
    }

    if ((semid = semget(key, 1, 0)) == -1)
    {
        fprintf(stderr, "%s: ", argv[0]);
        perror("semget Error");
        exit(1);
    }

    for (int i = 0; i < 5; i++)
    {
        // locking semophore
        if (semop(semid, &sb, 1) == -1)
        {
            fprintf(stderr, "%s: ", argv[0]);
            perror("semop Error");
            exit(1);
        }

        // entering critical section

        sleepTime = rand() % 6;
        sleep(sleepTime);

        time(&current_time);
        time_info = localtime(&current_time);
        strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);

        fptr = fopen(logFile, "a");
        fprintf(fptr, "%s Child process %i entered critical section on turn %i\n", timeString, procNum, i);
        fclose(fptr);

        fptr = fopen("cstest", "a");
        fprintf(fptr, "%s Queue %i File modified by process number %i\n", timeString, i, procNum);
        fclose(fptr);

        sleepTime = rand() % 6;
        sleep(sleepTime);

        // leaving critical section

        time(&current_time);
        time_info = localtime(&current_time);
        strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);

        fptr = fopen(logFile, "a");
        fprintf(fptr, "%s Child process %i left critical section on turn %i\n", timeString, procNum, i);
        fclose(fptr);

        // unlocking semephore
        sb.sem_op = 1; /* free resource */
        if (semop(semid, &sb, 1) == -1)
        {
            fprintf(stderr, "%s: ", argv[0]);
            perror("semop Error");
            exit(1);
        }
    }
}
