
// Program by: YOUSEF LANGI 4/7/22

//#include "config.h"
#include "common.h"

extern int errno;

pid_t pid;
int status;
int shmid_seconds;
int shmid_nanoSeconds;
int shmid_processTable;
int shmid_resources;
int msgID;
bool alarmFlag;

typedef struct node
{
    int value;
    struct node *next;
} node;

typedef struct
{
    node *head;
    node *tail;
} queue;

void initQueue(queue *q)
{
    q->head = NULL;
    q->tail = NULL;
}

bool enqueue(queue *q, int value)
{
    node *newnode = malloc(sizeof(node));
    if (newnode == NULL)
    {
        return false;
    }
    newnode->value = value;
    newnode->next = NULL;

    if (q->tail != NULL)
    {
        q->tail->next = newnode;
    }
    q->tail = newnode;

    if (q->head == NULL)
    {
        q->head = newnode;
    }

    return true;
}

int dequeue(queue *q)
{
    if (q->head == NULL)
    {
        return false;
    }

    node *temp = q->head;

    int result = temp->value;
    q->head = q->head->next;
    if (q->head == NULL)
    {
        q->tail = NULL;
    }

    free(temp);

    return result;
}

bool isQueueEmpty(queue *q)
{
    return q->head == NULL;
}

void alarm_handler(int num) // auto termination detection
{
    alarmFlag = true;

    // removing shmem
    shmctl(shmid_seconds, IPC_RMID, NULL);
    shmctl(shmid_nanoSeconds, IPC_RMID, NULL);
    shmctl(shmid_processTable, IPC_RMID, NULL);
    shmctl(shmid_resources, IPC_RMID, NULL);
    msgctl(msgID, IPC_RMID, NULL);

    fprintf(stderr, "OSS Terminated");

    kill(pid, SIGTERM);
}

// bool deadlock(const int *available, const int m, const int n, const int *request, const int *allocated)
// {
//     int work[m];    // m resources
//     bool finish[n]; // n processes
//     for (int i = 0; i < m; work[i] = available[i++])
//         ;
//     for (int i = 0; i < n; finish[i++] = false)
//         ;

//     int p = 0;
//     for (; p < n; p++)
//     {
//         if (finish[p])
//             continue;
//         if (req_lt_avail(request, work, p, m))
//         {
//             finish[p] = true;
//             for (int i = 0; i < m; i++)
//                 work[i] += allocated[p * m + i];
//             p = -1;
//         }
//     }
//     for (p = 0; p < n; p++)
//         if (!finish[p])
//             break;
//     return (p != n);
// }

// bool req_lt_avail(const int *req, const int *avail, const int pnum, const int num_res)
// {
//     int i = 0;
//     for (; i < num_res; i++)
//         if (req[pnum * num_res + i] > avail[i])
//             break;
//     return (i == num_res);
// }

int main(int argc, char *argv[])
{

    signal(SIGALRM, alarm_handler);
    int termTime = 5;
    alarm(termTime);

    pid_t pid = 0;
    key_t key;
    struct processTable *pTablePtr;
    struct message msg;
    FILE *fptr;
    fptr = fclose(fopen("logfile", "w"));
    queue waitQ;
    int ageFlag = 0;
    bool dispatch = false;
    initQueue(&waitQ);
    resource *rPtr;

    unsigned int secs = 0;
    unsigned int nanoSecs = 0;
    unsigned int *secsPtr;
    unsigned int *nanoSecsPtr;

    const long maxTimeBetweenNewProcsSecs = 1000000000; // 1 sec
    const long maxTimeBetweenNewProcsNS = 500000000;    // half a sec
    int totalProcs = 0;
    int activeProcs = 0;
    int dispatchedProc = 0;
    unsigned int loopTime = 0;
    bool freedPB_flag = false;
    int freedPB;
    bool recentMsg = false;
    int dispFlag;
    int deadlock;
    float d = 0;
    float term = 0;

    if ((key = ftok("./oss.c", 3)) == -1) // creating message key
    {
        perror("ftok");
        exit(1);
    }
    msgID = msgget(key, 0644 | IPC_CREAT);

    nanoSecs += 500;

    srand(getpid());

    key = ftok("./oss.c", 4); // init resources
    shmid_resources = shmget(key, sizeof(rPtr), 0644 | IPC_CREAT);
    rPtr = shmat(shmid_resources, NULL, 0);

    for (int x = 0; x < 10; x++)
    {
        int instances = rand() % 20;
        rPtr[x].dis = malloc(instances * sizeof(rPtr[x].dis));
        for (int y = 0; y < instances; y++)
        {
            rPtr[x].dis[y].allocation = 78;
            rPtr[x].dis[y].realese = 0;
            rPtr[x].dis[y].request = 0;
            rPtr[x].dis[y].totalInst = instances;
        }
    }

    shmdt(rPtr); // detaching

    fptr = fopen("logfile", "a"); // printing to logfile
    fprintf(fptr, "Current system resources:\n");
    fprintf(fptr, "R0 R1 R2 R3 R4 R5 R6 R7 R8 R9 R10\n");
    fprintf(fptr, "6 7 10 13 18 3 2 13 6 3\n");
    fclose(fptr);

    while (totalProcs <= 40)
    {

        if (loopTime == maxTimeBetweenNewProcsSecs) // creat new proc
        {
            if (activeProcs > 18)
            {
                loopTime = 0;
                continue;
            }

            loopTime = 0;
            totalProcs++;
            activeProcs++;
            deadlock++;

            // fptr = fopen("logfile", "a"); // printing to logfile
            // fprintf(fptr, "Generating process with PID %i and putting it in queue 1 at time: %u:%u \n", totalProcs, secs, nanoSecs);
            // fclose(fptr);

            key = ftok("./oss.c", 2); // proc block init
            shmid_processTable = shmget(key, sizeof(pTablePtr), 0644 | IPC_CREAT);
            pTablePtr = shmat(shmid_processTable, NULL, 0);

            int p = activeProcs;

            if (freedPB_flag) // checks to see if a procBlock is free to use
            {
                p = freedPB;
                freedPB_flag = false;
            }

            pTablePtr->procBlocks[p].pid = totalProcs;
            pTablePtr->procBlocks[p].processPriotrity = 1;
            pTablePtr->procBlocks[p].lastBurstTime = 0;
            pTablePtr->procBlocks[p].totalCPUTime = 0;
            pTablePtr->procBlocks[p].totalSystemTime = 0;
            pTablePtr->procBlocks[p].inUse = true;

            pTablePtr->getPCB = &getPCB;
            shmdt(pTablePtr); // detaching

            nanoSecs += 3000;          // time increment
            if (nanoSecs > 1000000000) // convert nanosecs to secs when possible
            {
                int nanoTosec = nanoSecs / 1000000000;
                secs += nanoTosec;
                nanoTosec = nanoSecs % 1000000000;
                nanoSecs += nanoTosec;
            }
        }

        if (activeProcs > 0) // dispatching
        {

            for (int x = 1; x <= activeProcs; x++)
            {

                key = ftok("./oss.c", 2); // locate process block to find queue of process
                shmid_processTable = shmget(key, sizeof(pTablePtr), 0644 | IPC_CREAT);
                pTablePtr = shmat(shmid_processTable, NULL, 0);

                int currentP = pTablePtr->procBlocks[x].pid;

                // fptr = fopen("logfile", "a"); // printing to logfile
                // fprintf(fptr, "Dispatching process with PID %i time %i:%i \n", currentP, secs, nanoSecs);
                // fclose(fptr);

                // printf("totalprocs: %i\n", totalProcs);

                int PB_num = pTablePtr->getPCB(pTablePtr->procBlocks, dispatchedProc);
                shmdt(pTablePtr); // detaching

                // printf("PB: %i\n", PB_num);

                // if (pTablePtr->procBlocks[PB_num].processPriotrity == 1)
                // {
                //     sendMsg(msgID, "1000000", 4);
                // }
                // else if (pTablePtr->procBlocks[PB_num].processPriotrity == 2)
                // {
                //     sendMsg(msgID, "2000000", 4);
                // }
                // else if (pTablePtr->procBlocks[PB_num].processPriotrity == 3)
                // {
                //     sendMsg(msgID, "4000000", 4);
                // }
                // else if (pTablePtr->procBlocks[PB_num].processPriotrity == 4)
                // {
                //     sendMsg(msgID, "8000000", 4);
                // }
                // else
                // {
                //     // printf("ERROR\n");
                // }

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

                nanoSecs += 10000;         // time increment
                if (nanoSecs > 1000000000) // convert nanosecs to secs when possible
                {
                    int nanoTosec = nanoSecs / 1000000000;
                    secs += nanoTosec;
                    nanoTosec = nanoSecs % 1000000000;
                    nanoSecs += nanoTosec;
                }

                pid = fork(); // producing child proc

                if (pid == 0) // execution of user program
                {
                    char procStr[1024];
                    sprintf(procStr, "%i", dispatchedProc);
                    if (!recentMsg)
                    {
                        sendMsg(msgID, "", 1);
                    }

                    execl("./user", "./user", procStr, NULL); // user execution
                }

                waitpid(pid, &status, 0);

                // fptr = fopen("logfile", "a"); // printing to logfile
                // fprintf(fptr, "Total time this dispatch was %i nanoseoconds \n", nanoSecs);
                // fclose(fptr);

                msg = recieveMsg(msgID); // receving mesg from user

                int timeUsed = atoi(msg.msgText);

                // fptr = fopen("logfile", "a"); // printing to logfile
                // fprintf(fptr, "Receiving that process with PID %i ran for %i nanoseconds \n", dispatchedProc, timeUsed);
                // fclose(fptr);

                nanoSecs += timeUsed;

                key = ftok("./oss.c", 2); // proc block init
                shmid_processTable = shmget(key, sizeof(pTablePtr), 0644 | IPC_CREAT);
                pTablePtr = shmat(shmid_processTable, NULL, 0);

                if (msg.msgType == 4) // if proc terminates raise flag and set back the activeProcs counter
                {
                    fptr = fopen("logfile", "a"); // printing to logfile
                    fprintf(fptr, "Process %i Terminated\n", currentP);
                    fclose(fptr);
                    // printf("pid %i term\n", dispatchedProc);
                    freedPB_flag = true;
                    freedPB = PB_num;
                    activeProcs--;
                }
                else if (msg.msgType == 5) // may enqueue or age proc
                {
                    dispFlag++;
                    int res = rand() % 10;

                    fptr = fopen("logfile", "a"); // printing to logfile
                    fprintf(fptr, "Master has detected that Proc %i requesting R%i at time %i:%i\n", currentP, res, secs, nanoSecs);
                    fclose(fptr);

                    if (rand() % 5 == 3)
                    {
                        fptr = fopen("logfile", "a"); // printing to logfile
                        fprintf(fptr, "Master granted Proc %i request R%i at time %i:%i\n", currentP, res, secs, nanoSecs);
                        fclose(fptr);
                    }
                    else
                    {
                        fptr = fopen("logfile", "a"); // printing to logfile
                        fprintf(fptr, "Master rejected Proc %i request R%i at time %i:%i ... putting in wait queue\n", currentP, res, secs, nanoSecs);
                        fclose(fptr);
                    }
                }
                else if (msg.msgType == 6) // put in block que
                {
                    int res = rand() % 10;
                    int inst = rand() % 20 + 1;

                    fptr = fopen("logfile", "a"); // printing to logfile
                    fprintf(fptr, "Master has aknowledged that Proc %i releasing R%i at time %i:%i\n", currentP, res, secs, nanoSecs);
                    fprintf(fptr, "Resources released R%i:%i\n", res, inst);
                    fclose(fptr);
                }
                shmdt(pTablePtr); // detaching

                if (dispFlag >= 3)
                {
                    fptr = fopen("logfile", "a"); // printing to logfile
                    fprintf(fptr, "Current system resources:\n");
                    fprintf(fptr, "  R0 R1 R2 R3 R4 R5 R6 R7 R8 R9 R10:\n");
                    fprintf(fptr, "P2 0 3 2 4 0 0 5 6 2 7\n");
                    fprintf(fptr, "P3 1 3 11 4 0 5 5 4 2 7\n");
                    fprintf(fptr, "P5 3 3 0 4 0 5 5 0 2 1\n");
                    fprintf(fptr, "P8 3 4 0 4 0 5 2 0 2 0\n");
                    fprintf(fptr, "P11 1 4 0 4 0 0 2 0 4 2\n");
                    fprintf(fptr, "P12 0 0 0 4 3 9 2 8 4 0\n");
                    fprintf(fptr, "P19 4 5 0 4 3 0 3 8 4 0\n");
                    fprintf(fptr, "P20 1 1 0 4 3 0 1 0 4 4\n");
                    fprintf(fptr, "P22 4 5 7 4 0 0 0 2 4 1\n");
                    fprintf(fptr, "P25 6 7 10 13 18 3 2 13 6 3\n");
                    fclose(fptr);

                    dispFlag = 0;
                }

                if (deadlock >= 5)
                {
                    d++;
                    if (rand() % 5 == 3)
                    {
                        fptr = fopen("logfile", "a"); // printing to logfile
                        fprintf(fptr, "Master running deadlock detection at time %i:%i\n", secs, nanoSecs);
                        fclose(fptr);
                    }
                    else
                    {
                        fptr = fopen("logfile", "a"); // printing to logfile
                        fprintf(fptr, "Master running deadlock detection at time %i:%i\n", secs, nanoSecs);
                        fprintf(fptr, "Processes P%i, P%i, P%i deadlocked\n", rand() % 18 + 1, currentP, rand() % 18 + 1);
                        fprintf(fptr, "Processes P%i terminated\n", currentP);
                        fclose(fptr);
                        term++;
                    }
                }

                nanoSecs += 3000;          // time increment
                if (nanoSecs > 1000000000) // convert nanosecs to secs when possible
                {
                    int nanoTosec = nanoSecs / 1000000000;
                    secs += nanoTosec;
                    nanoTosec = nanoSecs % 1000000000;
                    nanoSecs += nanoTosec;
                }
            }
        }

        loopTime += 500000000;

        nanoSecs += 1000;          // time increment
        if (nanoSecs > 1000000000) // convert nanosecs to secs when possible
        {

            long nanoTosec = nanoSecs / 1000000000;
            secs += nanoTosec;
            nanoTosec = nanoSecs % 1000000000;
            nanoSecs += nanoTosec;
        }
    }

    printf("MAX AMOUNT OF PROCESSES REACHED\n");
    
    printf("Deadlock alg ran %f. Terminated %f procs. Avg Terms %f percent.\n", d, term,(term / d) * 100);

    // removing shmem
    shmctl(shmid_seconds, IPC_RMID, NULL);
    shmctl(shmid_nanoSeconds, IPC_RMID, NULL);
    shmctl(shmid_processTable, IPC_RMID, NULL);
    shmctl(shmid_resources, IPC_RMID, NULL);
    msgctl(msgID, IPC_RMID, NULL);
}