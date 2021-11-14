#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <string.h> 
#include <stddef.h>
#include <sys/un.h>

#define SERVER_PATH "server"
#define CLIENT_PATH "client"

int main() 
{
  struct sockaddr_un clientAddress;
  struct sockaddr_un serverAddress;
  char buffer[BUFSIZ];
  int clientSocket, binding, lengthServer;
  memset( & clientAddress, 0, sizeof(struct sockaddr_un));
  memset( & serverAddress, 0, sizeof(struct sockaddr_un));
  clientSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (clientSocket == -1) {
    perror("Connection Failed\n");
    exit(1);
  }
  clientAddress.sun_family = AF_UNIX;
  strcpy(clientAddress.sun_path, CLIENT_PATH);
  lengthServer = sizeof(clientAddress);
  unlink(CLIENT_PATH);
  binding = bind(clientSocket, (struct sockaddr * ) & clientAddress, lengthServer);
  if (binding == -1) {
    perror("Binding Failed\n");
    close(clientSocket);
    exit(1);
  }
  serverAddress.sun_family = AF_UNIX;
  strcpy(serverAddress.sun_path, SERVER_PATH);
  printf("give me the file path... ");
  scanf("%s", buffer);
  sendto(clientSocket, buffer, strlen(buffer), 0, (struct sockaddr * ) & serverAddress, sizeof(struct sockaddr * ));
  if (binding == -1) {
    perror("Sending Failed\n");
    close(clientSocket);
    exit(3);
  }
  memset(buffer, 0, sizeof(buffer));
  while ((binding = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
    printf("%s\n", buffer);
  }
  /* closing connections */
  close(clientSocket);
  return 0;
}
