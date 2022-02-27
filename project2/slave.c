#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <sys/wait.h>
// #include "shared_memory.h"

int main(int argc, char *argv[])
{
    // char *memBlock = attachMemoryBlock(FILENAME, BLOCKSIZE);
    // if (memBlock == NULL)
    // {
    //     fprintf(stderr, "error, no block2\n");
    //     return 1;
    // }
    // else
    // {
    //     fprintf(stderr, "block2 OK\n");
    //      fprintf(stderr, "block: %s\n", memBlock);
    // }

    bool *choosing;
    int shmid_choosing;
    int *number;
    int shmid_number;

    // shmem init
    key_t key;

    key = ftok("./master.c", 0);
    shmid_choosing = shmget(key, sizeof(choosing), 0644 | IPC_CREAT);
    choosing = shmat(shmid_choosing, NULL, 0);

    key = ftok("./master.c", 1);
    shmid_number = shmget(key, sizeof(number), 0644 | IPC_CREAT);
    choosing = shmat(shmid_choosing, NULL, 0);
    number = shmat(shmid_number, NULL, 0);


    FILE *fptr;


    int procNum = atoi(argv[1]);
    int max = 0;

    int childProc = getpid();
    srand(time(NULL));
    int sleepTime;
    struct timespec now;
    long curTime;

    struct tm *time_info;
    time_t current_time;
    char timeString[9]; // space for "HH:MM:SS\0"
    char logFile[10];
    snprintf(logFile, sizeof(char) * 10, "logfile.%i", procNum); // creating logfile.xx
   

    for (int i = 0; i < 5; i++)
    {
       // fprintf(stderr, "proc %s\n", argv[1]);
        choosing[procNum] = true;

        for (int i = 0; i < MAXSLAVEPROC; i++)
        {
            if ((number[i]) > max)
            {
                max = (number[i]);
            }
        }
        number[i] = 1 + max;

       // fprintf(stderr, "turnNum for process #%i = %i\n", procNum, number[i]);
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

    shmdt(choosing);
    shmdt(number);
    shmctl(shmid_choosing, IPC_RMID, NULL);
    shmctl(shmid_number, IPC_RMID, NULL);
}
