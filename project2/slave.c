
// Program by: YOUSEF LANGI 2/24/22

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
#include "config.h"

extern int errno;

int main(int argc, char *argv[])
{
    bool *choosing;
    int shmid_choosing;
    int *number;
    int shmid_number;
    int max = 0;
    int procNum = atoi(argv[1]);
    
    srand(time(NULL));
    int sleepTime;
    struct tm *time_info;
    time_t current_time;
    char timeString[9]; // space for "HH:MM:SS\0"

    FILE *fptr;
    char logFile[20];
    snprintf(logFile, sizeof(char) * 20, "logfile.%i", procNum); // creating logfile.xx
   

    // receiving initialized shmem
    key_t key;

    key = ftok("./master.c", 0);
    shmid_choosing = shmget(key, sizeof(choosing), 0644 | IPC_CREAT);
    choosing = shmat(shmid_choosing, NULL, 0);

    key = ftok("./master.c", 1);
    shmid_number = shmget(key, sizeof(number), 0644 | IPC_CREAT);
    choosing = shmat(shmid_choosing, NULL, 0);
    number = shmat(shmid_number, NULL, 0);

    if (shmid_number == -1)
    {
        fprintf(stderr, "%s", strerror(errno)); // incase of shmem error
    }

    for (int i = 0; i < 5; i++)
    {
        choosing[procNum] = true;

        for (int i = 0; i < MAXSLAVEPROC; i++)
        {
            if ((number[i]) > max)
            {
                max = (number[i]);
            }
        }
        number[i] = 1 + max;

        choosing[procNum] = false;

        for (int j = 0; j < MAXSLAVEPROC; j++)
        {
            while (choosing[j])
                ; // Wait if j happens to be choosing
            while ((number[j] != 0) && (number[j] < number[i]))
                ;
        }

        // critical section

        sleepTime = rand() % 6;
        sleep(sleepTime);
        

        time(&current_time);
        time_info = localtime(&current_time);
        strftime(timeString, sizeof(timeString), "%H:%M:%S", time_info);

        fptr = fopen(logFile, "a");
        fprintf(fptr, "%s Child process %i entered critical section on turn %i\n", timeString, procNum, number[i]);
        fclose(fptr);

        
        fptr = fopen("cstest", "a");
        fprintf(fptr, "%s Queue %i File modified by process number %i\n", timeString, number[i], procNum);
        fclose(fptr);

        sleepTime = rand() % 6;
        sleep(sleepTime);


        fptr = fopen(logFile, "a");
        fprintf(fptr, "%s Child process %i left critical section on turn %i\n", timeString, procNum, number[i]);
        fclose(fptr);

        number[i] = 0;
    }

    // detaching and removing smhmem
    shmdt(choosing);
    shmdt(number);
    shmctl(shmid_choosing, IPC_RMID, NULL);
    shmctl(shmid_number, IPC_RMID, NULL);
}
