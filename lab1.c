#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char *buffptr = NULL;
  size_t size = 0;

  printf("Please enter some text: ");

  getline(&buffptr, &size, stdin);

  printf("Tokens:");

  const char delim = ' ';
  char *saveptr = NULL;
  char *tokenStringptr = NULL;

  tokenStringptr = strtok_r(buffptr, &delim, &saveptr);
  if (tokenStringptr != NULL) {
    printf("\n%s", tokenStringptr);
  }

  while (1 == 1) { // basically an infinite loop until end of line, but true
                   // requires stdbool.h
    tokenStringptr = strtok_r(NULL, &delim, &saveptr);
    if (tokenStringptr != NULL) {
      printf("\n%s", tokenStringptr);
    } else {
      break; // exit loop once strtok_r returns NULL
    }
  }

  free(buffptr);

  return 0;
}
