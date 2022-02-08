#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

int main(int argc, char *argv[])
{

   pid_t childpid = 0;
   int i, numOfProcesses = 4, numOfChars = 80, sleepTime = 3, numOfIterations;

   int option;
   while ((option = getopt(argc, argv, "h:p:c:s:i")) != -1) // :pcsi
   {
      switch (option)
      {
      case 'h':
         fprintf(stderr, "Usage: %s [-h] [-p nprocs] [-c nchars] [-s sleeptime] [-i niters] < textfile \n", argv[0]);
         return 1;
         break;
      case 'p':
      fprintf(stderr, "proc %i \n", atoi(optarg));
         if(optarg != NULL)
         {
            numOfProcesses = atoi(optarg);
         }
         else
         {
            numOfProcesses = atoi(argv[3]);
         }
         
         
         break;
      case 'c':
       fprintf(stderr, "chars %i \n", numOfChars);
      if(optarg != NULL)
         {
            numOfChars = atoi(optarg);
         }
         else
         {
            numOfChars = atoi(argv[3]);
         }
         
         fprintf(stderr, "chars %i \n", numOfChars);
         break;
      case 's':
         sleepTime = atoi(argv[4]);
         fprintf(stderr, "sleep %i \n", sleepTime);
         break;
      case 'i':
         numOfIterations = atoi(argv[5]);
         fprintf(stderr, "iterations %i \n", numOfIterations);
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

   for (i = 1; i < numOfProcesses; i++)
      if (childpid = fork())
         break;

   fprintf(stderr, "i:%d process ID:%ld parent ID:%ld child ID:%ld\n",
           i, (long)getpid(), (long)getppid(), (long)childpid);

   return 0;
}