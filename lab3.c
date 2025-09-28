#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printArr(int startIndex, char **strArray) {
  for (int i = 0; i < 5; i++) {
    printf("%s", strArray[(startIndex + i) % 5]);
  }
  return;
}

int main() {
  char *strArray[5] = {"", "", "", "", ""}; // Array of 5 char* (strings)
  char *lineptr = NULL;
  size_t lineSize;
  int writeIndex = 0;
  int printStartIndex = 0;

  while (1) {
    printf("Enter input: ");
    getline(&lineptr, &lineSize, stdin);

    // Allocate memory and assign the string
    strArray[writeIndex % 5] = malloc(sizeof(char) * lineSize);
    strcpy(strArray[writeIndex % 5], lineptr);

    // Detect if need to print
    if (strcmp(lineptr, "print\n") == 0) {
      printArr(printStartIndex, strArray);
    }
    writeIndex++;

    // If weve looped back to the start of the array to overwrite old values,
    // move the start of the printing to preserve specifically the most recent 5
    // in old -> new order
    if (writeIndex > 4) {
      printStartIndex++;
    }
  }

  // This code will never run but it's here just to show how it would be freed
  free(lineptr); // free the buffer allocated by getline
  for (int i = 0; i < 5; i++) {
    free(strArray[i]); // free the memory of the string array
  }

  return 0;
}
