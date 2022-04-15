#include "config.h"
#include "common.h"

int main(int argc, char *argv[])
{

    // fprintf(stderr, "in user proc %s \n", argv[1]);

    key_t key;
    struct processTable *pTablePtr;
    struct message msg;
    srand(getpid());

    //  int msgID = atoi(argv[1]);
    int msgID;
    int shmid_processTable;
    int shmid_seconds;
    int shmid_nanoSeconds;
    unsigned int *secsPtr;
    unsigned int *nanoSecsPtr;

    if ((key = ftok("./user.c", 3)) == -1)
    {
        perror("ftok");
        exit(1);
    }
    msgID = msgget(key, 0644 | IPC_CREAT);

    strcpy(msg.msgText, recieveMsg(msgID));
    fprintf(stderr, "user recevied: %s\n", msg.msgText);

    msgctl(msgID, IPC_RMID, NULL);

    key = ftok("./oss.c", 0);
    shmid_seconds = shmget(key, sizeof(secsPtr), 0644 | IPC_CREAT);
    secsPtr = shmat(shmid_seconds, NULL, 0);
    fprintf(stderr, "secs: %i \n", secsPtr[0]);
    shmdt(secsPtr); // detaching

    key = ftok("./oss.c", 1);
    shmid_nanoSeconds = shmget(key, sizeof(nanoSecsPtr), 0644 | IPC_CREAT);
    nanoSecsPtr = shmat(shmid_nanoSeconds, NULL, 0);
    fprintf(stderr, "nanoSecs: %i \n", nanoSecsPtr[0]);
    shmdt(nanoSecsPtr); // detaching

    key = ftok("./oss.c", 2);
    shmid_processTable = shmget(key, sizeof(pTablePtr), 0644 | IPC_CREAT);
    pTablePtr = shmat(shmid_processTable, NULL, 0);
    shmdt(pTablePtr); // detaching

    if ((key = ftok("./user.c", 3)) == -1)
    {
        perror("ftok");
        exit(1);
    }
    msgID = msgget(key, 0644 | IPC_CREAT);
    sendMsg(msgID, "stop");
}