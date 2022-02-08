#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

int main(int argc, char *argv[])
{

   pid_t childpid = 0;
   int i, numOfProcesses = 4, numOfChars = 80, sleepTime = 3, numOfIterations;

   int option;
   while ((option = getopt(argc, argv, "hh:p:c:s:i:")) != -1) // :pcsi
   {
      switch (option)
      {
      case 'h':
         fprintf(stderr, "Usage: %s [-h] [-p nprocs] [-c nchars] [-s sleeptime] [-i niters] < textfile \n", argv[0]);
         return 1;
         break;
      case 'p':
         if (optarg != NULL)
         {
            numOfProcesses = atoi(optarg);
         }
         else
         {
            numOfProcesses = atoi(argv[3]);
         }
         //  fprintf(stderr, "proc %i \n", numOfProcesses);
         break;
      case 'c':
         if (optarg != NULL)
         {
            numOfChars = atoi(optarg);
         }
         else
         {
            numOfChars = atoi(argv[3]);
         }
         // fprintf(stderr, "chars %i \n", numOfChars);
         break;
      case 's':
         if (optarg != NULL)
         {
            sleepTime = atoi(optarg);
         }
         //fprintf(stderr, "sleep %i \n", sleepTime);
         break;
      case 'i':
         if (optarg != NULL)
         {
            numOfIterations = atoi(optarg);
         }
         // fprintf(stderr, "iterations %i \n", numOfIterations);
         break;
      default:
         //never enters
         break;
      }
   }

   if (argc < 2)
   {
      perror("Chain: Error: Try ./simplechain -h for help \n");
      return 1;
   }

   for (i = 1; i <= numOfProcesses; i++)
   {

      if (childpid = fork())
      {
         break;
      }

      for (int x = 0; x < numOfIterations; x++)
      {
         wait(sleepTime);
        // fprintf(stderr, "i:%d process ID:%ld parent ID:%ld child ID:%ld \n", i, (long)getpid(), (long)getppid(), (long)childpid);
         fprintf(stderr, "i:%d ", i);
         fprintf(stderr, "process ID:%ld ", (long)getpid());
         fprintf(stderr, "parent ID:%ld ", (long)getppid());
         fprintf(stderr, "child ID:%ld \n", (long)childpid);
      }
     // fprintf(stderr, "\n");
   }
   return 0;
}