#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h> 
#include <stddef.h>
#include <sys/un.h>
#include <pthread.h>


#define SERVER_PATH "server"
#define SLAVE_PATH1 "slave1"
#define SLAVE_PATH2 "slave2"
#define SLAVE_PATH3 "slave3"
#define SLAVE_PATH4 "slave4"
#define SLAVES_NUMBER 4

pthread_mutex_t mutex;

char buffer[BUFSIZ];

int commandsNumber = 0;
struct sockaddr_un serverAddress, clientSock;
int serverSockeet, lengthServer, binding;
typedef struct Datos {
  int idSlave;
  char content[1024];
}
Datos;
void * serve(void * args) {
  /* getting the argument */
  Datos data = * (Datos * ) args;
  /* declaring address of slave using the id sent via the argument */
  struct sockaddr_un slaveSockettaddr;
  int slaveSockett;
  slaveSockettaddr.sun_family = AF_UNIX;
  if (data.idSlave == 1)
    strcpy(slaveSockettaddr.sun_path, SLAVE_PATH1);
  if (data.idSlave == 2)
    strcpy(slaveSockettaddr.sun_path, SLAVE_PATH2);
  if (data.idSlave == 3)
    strcpy(slaveSockettaddr.sun_path, SLAVE_PATH3);
  if (data.idSlave == 4)
    strcpy(slaveSockettaddr.sun_path, SLAVE_PATH4);
  if (slaveSockett == -1) {
    perror("Cannot connect\n");
    exit(2);
  }
  sendto(serverSockeet, data.content, strlen(data.content), 0, (struct sockaddr * ) & slaveSockettaddr, sizeof(struct sockaddr * ));
  int helper = 0;
  while (helper == 0) {
    helper = recv(serverSockeet, buffer, sizeof(buffer), 0);
    buffer[helper] = '\0';
    printf("Slave said : %s\n", buffer);
    sendto(serverSockeet, buffer, strlen(buffer), 0, (struct sockaddr * ) & clientSock, sizeof(struct sockaddr * ));
  }
  return NULL;
}

int main() {
  int receivedBytes = 0;
  /* reserving memeory */
  memset( & serverAddress, 0, sizeof(struct sockaddr_un));
  memset(buffer, 0, BUFSIZ);
  serverSockeet = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (serverSockeet == -1) {
    perror("SOCKET ERROR\n");
    exit(1);
  }
  serverAddress.sun_family = AF_UNIX;
  strcpy(serverAddress.sun_path, SERVER_PATH);
  lengthServer = sizeof(serverAddress);
  unlink(SERVER_PATH);
  binding = bind(serverSockeet, (struct sockaddr * ) & serverAddress, lengthServer);
  if (binding == -1) {
    perror("Connection Failed\n");
    close(serverSockeet);
    exit(1);
  }
  printf("Server is running\n");
  receivedBytes = recvfrom(serverSockeet, buffer, 256, 0, (struct sockaddr * ) & clientSock, & lengthServer);
  if (receivedBytes == -1) {
    perror("SOCKET ERROR\n");
    close(serverSockeet);
    exit(1);
  } else {
    printf("Master said : %s\n", buffer);
    FILE * file;
    if((file = fopen(buffer,"r"))==NULL)
    {
        perror("File does not exist\n");
        exit(3);
    }
    int cch = 0;
    /* counting number of commands */
    /* purpose : create that number of threads and avoid conflict */
    do {
      cch = getc(file);
      if (cch == '\n')
        commandsNumber = commandsNumber + 1;
    } while (cch != EOF);
    int index = 0;
    fclose(file);
    file = fopen(buffer, "r");
    pthread_t * arr;
    arr = (pthread_t * ) malloc(sizeof(pthread_t) * commandsNumber);
    buffer[receivedBytes] = '\0';
    char content[1024];
    while (fgets(content, 1024, file)) {
      Datos * d;
      d = (Datos * ) malloc(sizeof(Datos));
      /* get the id between 1 and SLAVES_NUMBER */
      /* in order to make sure all slaves work */
      d -> idSlave = index % SLAVES_NUMBER + 1;
      /* get a line of code and send it binded in the argument */
      strcpy(d -> content, content);
      int res = pthread_create(arr + index, NULL, serve, d);
      if (res) {
        perror("Error thread");
        exit(5);
      }
      index++;
    }
    /* synchronisation */
    for (index = 0; index < commandsNumber; index++) {
      pthread_join(arr[index], NULL);
    }
  }
  /* closing connections */
  close(serverSockeet);
  return 0;
}
