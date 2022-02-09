#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <ctype.h>

int main(int argc, char *argv[])
{

   pid_t childpid = 0;
   int numOfProcesses = 4, numOfChars = 80, sleepTime = 3, numOfIterations = 1;
   int SIZE = 1000;
   char mybuf[SIZE];

   if (argc <= 1) // no input detected from cmd line
   {
      fprintf(stderr, "%s: ", argv[0] += 2);
      perror("Error");
      fprintf(stderr, "Try ./simplechain -h for help\n");
      return 1;
   }

   int option;
   while ((option = getopt(argc, argv, "i:p:c:s:h")) != -1)
   {
      switch (option)
      {

      case 'h':
         fprintf(stderr, "Usage: %s [-h] [-p nprocs] [-c nchars] [-s sleeptime] [-i niters] < textfile\n", argv[0]);
         return 1;
         break;

      case 'p':
         if (optarg != NULL && atoi(optarg) != 0)
         {
            numOfProcesses = atoi(optarg);
         }
         else
         {
            perror("Error");
            return 1;
         }
         fprintf(stderr, "proc %i \n", numOfProcesses);
         break;

      case 'c':
         if (optarg != NULL && atoi(optarg) != 0)
         {
            numOfChars = atoi(optarg);
         }
         else
         {
            perror("Error");
            return 1;
         }
         fprintf(stderr, "chars %i \n", numOfChars);
         break;

      case 's':
         if (optarg != NULL && atoi(optarg) != 0)
         {
            sleepTime = atoi(optarg);
         }
         else
         {
            perror("Error");
            return 1;
         }
         fprintf(stderr, "sleep %i \n", sleepTime);
         break;

      case 'i':
         if (optarg != NULL && atoi(optarg) != 0)
         {
            numOfIterations = atoi(optarg);
         }
         else
         {
            perror("Error");
            return 1;
         }
         fprintf(stderr, "iters %i \n", numOfIterations);
         break;

      default:
         //never enters
         return 1;
         break;
      }
   }

   if (optarg == NULL || atoi(optarg) == 0) // if delimiter '-' is used without option in cmd line
   {

      perror("Error");
      return 1;
   }

   for (int i = 1; i <= numOfProcesses; i++)
   {

      if (childpid = fork())
      {
         break;
      }

      for (int x = 0; x < numOfIterations; x++)
      {
         if (!isatty(0)) // if input redirection is detected
         {
            int z, ch;
            for (z = 0; z < numOfChars && ((ch = fgetc(stdin)) != EOF); z++)
            {
               if (isspace(ch)) // if white space detected do not count as char and skip
               {
                  z -= 1;
                  continue;
               }

               mybuf[z] = ch;
            }

            mybuf[z] = '\0';

            sleep(sleepTime);
            wait(NULL);
            fprintf(stderr, "%ld : %s\n", (long)getpid(), mybuf);
         }
         else // if no input redirection is detect
         {
            sleep(sleepTime);
            wait(NULL);
            fprintf(stderr, "i:%d ", i);
            fprintf(stderr, "process ID:%ld ", (long)getpid());
            fprintf(stderr, "parent ID:%ld ", (long)getppid());
            fprintf(stderr, "child ID:%ld\n", (long)childpid);
            //fprintf(stderr, "i:%d process ID:%ld parent ID:%ld child ID:%ld \n", i, (long)getpid(), (long)getppid(), (long)childpid);
         }
      }
   }
   return 0;
}