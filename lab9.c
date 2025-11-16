
/*
Questions to answer at top of client.c:
(You should not need to change the code in client.c)
1. What is the address of the server it is trying to connect to (IP address and
port number).
2. Is it UDP or TCP? How do you know?
3. The client is going to send some data to the server. Where does it get this
data from? How can you tell in the code?
4. How does the client program end? How can you tell that in the code?
*
* 1: IP: 127.0.0.1. Port: 8000
* 2: It's TCP because the client is using a SOCK_STREAM instead of a SOCK_DGRAM
* 3: It gets this data from stdin, which you can tell because of the program
calling read() on
* STDIN_FILENO and then using that input for the write() to the server
* 4: It ends once the while loop that keeps reading terminates, which happens
when 1 or fewer characters is sent (ie: when the user hits enter without typing
anything else, which just sends \n, which is one character)
*/

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000
#define BUF_SIZE 64
#define ADDR "127.0.0.1"

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

int main() {
  struct sockaddr_in addr;
  int sfd;
  ssize_t num_read;
  char buf[BUF_SIZE];

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, ADDR, &addr.sin_addr) <= 0) {
    handle_error("inet_pton");
  }

  int res = connect(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
  if (res == -1) {
    handle_error("connect");
  }

  while ((num_read = read(STDIN_FILENO, buf, BUF_SIZE)) > 1) {
    if (write(sfd, buf, num_read) != num_read) {
      handle_error("write");
    }
    printf("Just sent %zd bytes.\n", num_read);
  }

  if (num_read == -1) {
    handle_error("read");
  }

  close(sfd);
  exit(EXIT_SUCCESS);
}

#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 64
#define PORT 8000
#define LISTEN_BACKLOG 32

#define handle_error(msg)                                                      \
  do {                                                                         \
    perror(msg);                                                               \
    exit(EXIT_FAILURE);                                                        \
  } while (0)

// Shared counters for: total # messages, and counter of clients (used for
// assigning client IDs)
int total_message_count = 0;
int client_id_counter = 1;

// Mutexs to protect above global state.
pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t client_id_mutex = PTHREAD_MUTEX_INITIALIZER;

struct client_info {
  int cfd;
  int client_id;
};

void *handle_client(void *arg) {
  struct client_info *client = (struct client_info *)arg;

  int Fd = client->cfd;
  int clientId = client->client_id;

  printf("New client created! ID %d on socket FD %d\n", clientId, Fd);

  char buf[BUF_SIZE];
  int bytes_read = 0;

  while ((bytes_read = read(Fd, buf, BUF_SIZE)) > 0) {
    pthread_mutex_lock(&count_mutex);
    total_message_count += 1;
    printf(
        "Message received from client %d which is message number %d and says: ",
        clientId, total_message_count);
    pthread_mutex_unlock(&count_mutex);
    fflush(stdout); // Required to make sure it actually prints out
    write(STDOUT_FILENO, buf, bytes_read);
  }

  close(Fd);
  printf("Client %d is done!\n", clientId);

  return NULL;
}

int main() {
  struct sockaddr_in addr;
  int sfd;

  sfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sfd == -1) {
    handle_error("socket");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
    handle_error("bind");
  }

  if (listen(sfd, LISTEN_BACKLOG) == -1) {
    handle_error("listen");
  }

  for (;;) {
    int newSocketFd = accept(sfd, NULL, NULL);

    struct client_info *clientInfoPtr = malloc(sizeof(struct client_info));
    clientInfoPtr->cfd = newSocketFd;

    pthread_mutex_lock(&client_id_mutex);
    clientInfoPtr->client_id = client_id_counter;
    client_id_counter += 1;
    pthread_mutex_unlock(&client_id_mutex);

    pthread_t t1;
    pthread_create(&t1, NULL, handle_client, clientInfoPtr);
    pthread_detach(t1);
  }

  if (close(sfd) == -1) {
    handle_error("close");
  }

  return 0;
}
