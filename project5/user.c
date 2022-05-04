//#include "config.h"
#include "common.h"

int main(int argc, char *argv[])
{

    // fprintf(stderr, "in user proc %s \n", argv[1]);

    key_t key;
    struct processTable *pTablePtr;
    resource *rPtr;
    struct message msg;

    int dispatchedProc = atoi(argv[1]);
    int msgID;
    int shmid_processTable;
    int shmid_seconds;
    int shmid_nanoSeconds;
    int shmid_resources;
    unsigned int secs = 0;
    unsigned int nanoSecs = 0;
    unsigned int *secsPtr;
    unsigned int *nanoSecsPtr;
    int timeSlice;
    srand(getpid());

    // shared memory retrival
    key = ftok("./oss.c", 0);
    shmid_seconds = shmget(key, sizeof(secsPtr), 0644 | IPC_CREAT);
    secsPtr = shmat(shmid_seconds, NULL, 0);
    secs = secsPtr[0];
    // fprintf(stderr, "secs: %i \n", secsPtr[0]);
    shmdt(secsPtr); // detaching

    key = ftok("./oss.c", 1);
    shmid_nanoSeconds = shmget(key, sizeof(nanoSecsPtr), 0644 | IPC_CREAT);
    nanoSecsPtr = shmat(shmid_nanoSeconds, NULL, 0);
    nanoSecs = nanoSecsPtr[0];
    // fprintf(stderr, "nanoSecs: %i \n", nanoSecsPtr[0]);
    shmdt(nanoSecsPtr); // detaching

    if ((key = ftok("./oss.c", 3)) == -1) // creating key for message
    {
        perror("ftok");
        exit(1);
    }
    msgID = msgget(key, 0644 | IPC_CREAT);

    msg = recieveMsg(msgID);

    // fprintf(stderr, "user recived %s\n", msg.msgText);
    // printf("proc: %i\n", dispatchedProc);

    key = ftok("./oss.c", 4); // init resources
    shmid_resources = shmget(key, sizeof(rPtr), 0644 | IPC_CREAT);
    rPtr = shmat(shmid_resources, NULL, 0);

    if (msg.msgType == 1) // has not communicated with oss recently
    {
        int userActionTime = rand() % 250000000;
        int futureNanoSecs = userActionTime + nanoSecs;

        int loopTime = nanoSecs;
        while (loopTime < futureNanoSecs) // waiting for future time
        {
            loopTime += 50000;
        }

        if (rand() % 10 == 5) // term possibility
        {
            sendMsg(msgID, "term", 4);
        }
        else if (rand() % 2 == 0) // request possibility
        {
            int res = rand() % 10;
            int inst = rand() % 20 + 1;

            sendMsg(msgID, "req", 5);
        }
        else // release
        {
             int res = rand() % 10;
            int inst = rand() % 20 + 1;
            sendMsg(msgID, "relesed", 6);
        }
    }
    else if (msg.msgType == 2) // resource granted
    {
        // take hold of resource
    }
     shmdt(rPtr); // detaching

}