#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  while (1) {
    printf("Enter programs to run.\n");

    char *lineptr = NULL;
    size_t len = 0;
    getline(&lineptr, &len, stdin);

    int forkVal = fork();
    if (forkVal == 0) {

      // Trim away the \n from user input, if there is one
      int strLen = strlen(lineptr);
      if (lineptr[strLen - 1] == '\n') {
        lineptr[strLen - 1] = '\0';
      }

      execl(lineptr, lineptr, (char *)NULL);
      printf("Exec failure\n"); // If the execution got to here, exec failed
    } else {

      int wStatus;
      waitpid(forkVal, &wStatus, 0);
    }

    free(lineptr);
  }
}
