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
//#include "config.h"

struct processBlock
{
    int pid;
    unsigned int totalCPUTime;
    unsigned int totalSystemTime;
    unsigned int lastBurstTime;
    unsigned int waitTime;
    int processPriotrity;
    bool inUse;
};

typedef int (*getPCBptr)(struct processBlock[], int);

int getPCB(struct processBlock procs[], int currentProc)
{

    for (int p = 0; p < 18; p++)
    {
        if (procs[p].pid == currentProc)
        {
            fprintf(stderr, "");
            return p;
        }
    }
}

struct processTable
{
    struct processBlock procBlocks[18];
    getPCBptr getPCB;
};

struct message
{
    long msgType; // 1: noMsg, 2: granted resource, 3: not granted, in wait queue, 4: user term, 5: user asked resource, 6: user released resource
    char msgText[1024];
};

typedef struct discriptor
{
    int request;
    int allocation;
    int realese;
    int totalInst;
}discriptor;

typedef struct resource
{
    discriptor *dis;
}resource;


void sendMsg(int msgID, char *m, int mType)
{

    struct message msg;
    strcpy(msg.msgText, m);
    msg.msgType = mType;
   // fprintf(stderr, "sending %li\n", msg.msgType);
    // printf("Size: %lu\n", sizeof(msg.msgText));

    // fprintf(stderr, "method sending: %i\n", strlen(msg.msgText));
    if (msgID == -1)
    {
        perror("mesgid");
        exit(1);
    }
    if (msgsnd(msgID, &msg, sizeof(msg.msgText), 0) == -1)
    {
        perror("msgsnd");
        exit(1);
    }
}

struct message recieveMsg(int msgID)
{
    struct message msg;


    if(msgrcv(msgID, &msg, sizeof(msg.msgText), 0, 0) == -1)
    {
        printf("no msg\n");
        
    }
    // char *m = msg.msgText;
    return msg;
}

#endif