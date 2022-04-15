#ifndef COMMON_H
#define COMMON_H

#include <sys/ipc.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
#include <sys/msg.h>
#include <string.h>
#include "config.h"

struct processBlock
{
    pid_t pid;
    unsigned int totalCPUTime;
    unsigned int totalSystemTime;
    unsigned int burstTime;
    int processPriotrity;
    bool inUse;
};

typedef int (*getPCBptr)(struct processBlock[]);

int getPCB(struct processBlock procs[])
{
    pid_t currentPID = getpid();

    for (int p = 0; p < 18; p++)
    {
        if (currentPID == procs[p].pid)
        {
            fprintf(stderr, "match %i \n", p);
            return p;
        }
        else
        {
            fprintf(stderr, "hi\n");
        }
    }

    return 0;
}

struct processTable
{
    struct processBlock procBlocks[18];
    getPCBptr getPCB;
};

struct message
{
    long msgType;
    char msgText[1024];
};

void sendMsg(int msgID, char *m)
{

    struct message msg;
    msg.msgType = 1;
    strcpy(msg.msgText, m);

 //fprintf(stderr, "method sending: %i\n", strlen(msg.msgText));
    if (msgID == -1)
    {
        perror("mesgid");
        exit(1);
    }
    if (msgsnd(msgID, &msg, strlen(msg.msgText), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
    if (msgsnd(msgID, &msg, 0, 0) == -1)
    {
        perror("msgsnd2");
        exit(1);
    }
}

char* recieveMsg(int msgID)
{
    struct message msg;
    msgrcv(msgID, &msg, sizeof(msg), 1, 0);
   // fprintf(stderr, "method reveived: %s\n", msg.msgText);
    return msg.msgText;
}

#endif