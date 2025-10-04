// Used ChatGPT to help figure out how to get the linter to recognize sbrk().
// Apparently sbrk() is not part of _POSIX_C_SOURCE, so the typical line of
// #define _POSIX_C_SOURCE 200809L doesnt work Also verified after that
// _DEFAULT_SOURCE is shown in the man page for sbrk
#define _DEFAULT_SOURCE

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct header {
  uint64_t size;
  struct header *next;
};

int BUF_SIZE = 100; // Placed globally just to make print_out work this way

void print_out(char *format, void *data, size_t data_size) {
  char buf[BUF_SIZE];
  ssize_t len = snprintf(buf, BUF_SIZE, format,
                         data_size == sizeof(uint64_t) ? *(uint64_t *)data
                                                       : *(void **)data);
  if (len < 0) {
    perror("snprintf");
  }
  write(STDOUT_FILENO, buf, len);
}

int main() {

  void *startAddress = sbrk(0);

  sbrk(256);

  struct header *header1 = startAddress;
  header1->size = 128;
  header1->next = NULL;
  memset(header1 + 1, 0, 128 - sizeof(struct header));

  struct header *header2 = startAddress + 128;
  header2->size = 128;
  header2->next = header1;
  memset(header2 + 1, 1, 128 - sizeof(struct header));

  print_out("First block starting address: %p\n", &header1, sizeof(&header1));
  print_out("Second block starting address: %p\n", &header2, sizeof(&header2));

  print_out("First block's size: %d\n", &header1->size, sizeof(uint64_t));
  print_out("First block's next: %p\n", &header1->next,
            sizeof(struct header *));

  print_out("Second block's size: %d\n", &header2->size, sizeof(uint64_t));
  print_out("Second block's next: %p\n", &header2->next,
            sizeof(struct header *));

  char *start = (char *)header1;
  char *printAddress = start + sizeof(struct header);
  for (int i = 0; i < 128 - sizeof(struct header); i++) {
    printAddress = start + sizeof(struct header) + i;
    int val = (int)*printAddress;
    print_out("%d\n", &val, sizeof(val));
  }

  start += 128;
  printAddress = start + sizeof(struct header);
  for (int i = 0; i < 128 - sizeof(struct header); i++) {
    printAddress = start + sizeof(struct header) + i;
    int val = (int)*printAddress;
    print_out("%d\n", &val, sizeof(val));
  }

  return 0;
}
