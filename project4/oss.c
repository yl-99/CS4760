
// Program by: YOUSEF LANGI 4/7/22

#include "config.h"
#include "common.h"

extern int errno;

pid_t pid;
int status;
int shmid_seconds;
int shmid_nanoSeconds;
int shmid_processTable;
int msgID;
bool alarmFlag;

void alarm_handler(int num) // auto termination detection
{
    alarmFlag = true;

    // removing shmem
    shmctl(shmid_seconds, IPC_RMID, NULL);
    shmctl(shmid_nanoSeconds, IPC_RMID, NULL);
    shmctl(shmid_processTable, IPC_RMID, NULL);
    msgctl(msgID, IPC_RMID, NULL);

    fprintf(stderr, "OSS Terminated");

    kill(pid, SIGTERM);
}
int main(int argc, char *argv[])
{

    pid_t pid = 0;
    key_t key;
    struct processTable *pTablePtr;
    struct message msg;

    // long (*fun)(struct processBlock[]) = &getPCB;
    unsigned int secs = 0;
    unsigned int nanoSecs = 0;
    unsigned int *secsPtr;
    unsigned int *nanoSecsPtr;

    int termTime = 3;
    int totalProcs = 0;
    unsigned int loopTime = 0;

    secs += 100000000;
    nanoSecs += 100000000;

    signal(SIGALRM, alarm_handler);
    while (!alarm(termTime) || totalProcs < 20)
    {

        if (loopTime == maxTimeBetweenNewProcsSecs)
        {
            if (totalProcs > 19) // checks if exceeded number of procs for proc table
            {
                loopTime = 0;
                continue;
            }

            loopTime = 0;

            // fprintf(stderr, "proc: %i\n", totalProcs);
            // shmem init

            key = ftok("./oss.c", 0);
            shmid_seconds = shmget(key, sizeof(secsPtr), 0644 | IPC_CREAT);
            secsPtr = shmat(shmid_seconds, NULL, 0);
            secsPtr[0] = secs;
            shmdt(secsPtr); // detaching

            key = ftok("./oss.c", 1);
            shmid_nanoSeconds = shmget(key, sizeof(nanoSecsPtr), 0644 | IPC_CREAT);
            nanoSecsPtr = shmat(shmid_nanoSeconds, NULL, 0);
            nanoSecsPtr[0] = nanoSecs;
            shmdt(nanoSecsPtr); // detaching

            // if (shmid_number == -1) // incase of shmem error
            // {
            //     fprintf(stderr, "%s", strerror(errno));
            // }

            pid = fork();
            totalProcs++;

            if (pid == 0) // execution of user program
            {
                key = ftok("./oss.c", 2); // proc block init
                shmid_processTable = shmget(key, sizeof(pTablePtr), 0644 | IPC_CREAT);
                pTablePtr = shmat(shmid_processTable, NULL, 0);
                for (int p = 0; p < 18; p++)
                {
                    pTablePtr->procBlocks[p].pid = getpid();
                    pTablePtr->procBlocks[p].processPriotrity = 0;
                    pTablePtr->procBlocks[p].burstTime = 0;
                    pTablePtr->procBlocks[p].totalCPUTime = 0;
                    pTablePtr->procBlocks[p].totalSystemTime = 0;
                    pTablePtr->procBlocks[p].inUse = false;
                }
                pTablePtr->getPCB = &getPCB;
                // pTablePtr->getPCB(pTablePtr->procBlocks);
                // fun(pTablePtr->procBlocks);
                shmdt(pTablePtr); // detaching
                // fprintf(stderr, "parent proc %ld \n", (long)getppid());
                // fprintf(stderr, "child proc %ld \n", (long)getpid());

                if ((key = ftok("./user.c", 3)) == -1)  // sending message to user
                {
                    perror("ftok");
                    exit(1);
                }
                msgID = msgget(key, 0644 | IPC_CREAT); 
                sendMsg(msgID, "100000");

                char msgIDStr[1024];
                sprintf(msgIDStr, "%d", msgID);

                execl("./user", "./user", msgIDStr, NULL); //user execution
            }

            waitpid(pid, &status, 0);
            if ((key = ftok("./user.c", 3)) == -1)
            {
                perror("ftok");
                exit(1);
            }
            msgID = msgget(key, 0644 | IPC_CREAT);
            strcpy(msg.msgText, recieveMsg(msgID));
            fprintf(stderr, "parent recived %s\n", msg.msgText);
            msgctl(msgID, IPC_RMID, NULL);
        }

        loopTime += 500000000;
        secs += 300000000;
        nanoSecs += 300000000;
    }

    // removing shmem
    shmctl(shmid_seconds, IPC_RMID, NULL);
    shmctl(shmid_nanoSeconds, IPC_RMID, NULL);
    shmctl(shmid_processTable, IPC_RMID, NULL);
    msgctl(msgID, IPC_RMID, NULL);
}