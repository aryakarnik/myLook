#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#define MAX_FILE_NAME_LEN 256
#define MINARG 1
#define MAXARG 2
#define MAX_CHAR_PER_LINE 300

/* opens the file that user wants to look into */
void OpenTheFile(int *fd, char *dict, struct stat *fileStat) {
   *fd = open(dict, O_RDONLY, 0);
   
   if (*fd < 0) {
      printf("Cannot open file; No such file available");
   }

   if (fstat(*fd, fileStat) < 0) {
      printf("Stat cannot be made");
   }
}

/* returns the address of the first character in a line 
 that contains the middle character of the file */
char *GetStartOfLine(char *middle, char *top) {
   while (*middle != '\n') {
      if (middle == top) {
         return middle;
      }
      middle--;
   }
   ++middle;
   return middle;
}

/* Searches through the file using binary search to narrow
 down the section that linear search has to search through */
char *BinarySearch(char *top, char *bottom, char *stringToLookFor, 
 char **newBottom, char *endOfFile, int fArg, int dArg) {
   char *middle = NULL;
   char buff[MAX_CHAR_PER_LINE];
   int index = 0;
   char *prevMiddle = NULL;

   memset(buff, 0, sizeof(buff));
   middle = top + ((bottom - top) / 2);
   middle = GetStartOfLine(middle, top);

   while (middle < bottom && bottom > top) {
      prevMiddle = middle;
      while (middle && middle <= endOfFile && *middle != '\n') {
         buff[index++] = *middle;
         middle++;
         if (middle >= endOfFile) {
            return top;
         }
      }
      buff[index] = '\0';

      if (strncmp(buff, stringToLookFor, strlen(stringToLookFor)) < 0) {
         top = prevMiddle;
      }
      else {
         bottom = prevMiddle;
      }
      middle = top + ((bottom - top) / 2);
      middle = GetStartOfLine(middle, top);
   }
   return top;
}

/* Find the final lines that need to be printed 
 using linear search. The flags are accounted for
 in this function */
void FindFinalMatchesAndPrint(char *top, char *bottom, 
 char *stringToLookFor, char *endOfFile, int fArg, int dArg) {
   char buff[MAX_CHAR_PER_LINE];
   char buff2[MAX_CHAR_PER_LINE];
   int index, index2;
   char *walker = NULL;

   while (top <= bottom) {
      memset(buff, 0, sizeof(buff));
      memset(buff2, 0, sizeof(buff2));
      walker = top;
      index = 0;
      index2 = 0;
      
      while (walker && (walker <= endOfFile) && (*walker != '\n')) {
         if (dArg) {
            if (isalnum(*walker)) {
               buff[index++] = *walker;
            }
            buff2[index2++] = *walker;
            walker++;
         }
         else if (!dArg) {
            buff[index++] = *walker;
            walker++;
         }
      }      

      if (fArg) {
         if (strncasecmp(buff, stringToLookFor, 
          strlen(stringToLookFor)) == 0) {
            if (dArg) {
               printf("%s\n", buff2);
            }
            else if (!dArg) {
               printf("%s\n", buff);
            }
         }
      }

      else if (!fArg) {
         if (strncmp(buff, stringToLookFor, strlen(stringToLookFor)) == 0) {
            if (dArg) {
               printf("%s\n", buff2);
            }
            else if (!dArg) {
               printf("%s\n", buff);
            }
         }
      }

      top += strlen(buff) + 1;
   }
}

int main(int argc, char *argv[]) {
   int arg = 0, dArg = 0, fArg = 0, tArg = 0, fd;
   int terminationChar = '\0'; 
   char def_dict[MAX_FILE_NAME_LEN] = "/usr/share/dict/words";
   char *dict = def_dict;
   char *stringToLookFor = NULL, *endOfFile = NULL;
   struct stat fileStat;
   char *top, *bottom, *replace_char, *newBottom = NULL;
   int nonOptArg = 0, binSearch = 0;

   while ((arg = getopt(argc, argv, "dft:")) != -1) {
      switch(arg) {
      case 'd':
         dArg = 1;
         break;
      case 'f':
         fArg = 1;
         break;
      case 't':
         tArg = 1;
         terminationChar = *optarg;
         break;
      case '?':
         break;
      default:
         printf("usage: look [-df] [-t char] string [file]\n");
         break;
      }
   }

   nonOptArg = argc - optind;

   if (nonOptArg < MINARG || nonOptArg > MAXARG) {
      printf("usage: look [-dfa] [-t char] string [file]\n");
      exit(2);
   }

   if (nonOptArg >= 1) {
       stringToLookFor = argv[optind++];
   }

   if (nonOptArg == 2) {
       dict = argv[optind];
   }

   if (terminationChar != '\0') {
       if ((replace_char = strchr(stringToLookFor, terminationChar)) != NULL) {
           *(replace_char + 1) = '\0';
       }
   }

   OpenTheFile(&fd, dict, &fileStat);
   top = mmap(NULL, (size_t) fileStat.st_size, 
    PROT_READ, MAP_SHARED, fd, (off_t) 0);
   bottom = top + fileStat.st_size;
   endOfFile = bottom;

   if (binSearch == 1) {
      top = BinarySearch(top, bottom, stringToLookFor, 
       &newBottom, endOfFile, fArg, dArg);
   }

   FindFinalMatchesAndPrint(top, bottom, stringToLookFor, 
    endOfFile, fArg, dArg);

   return 0;
}
