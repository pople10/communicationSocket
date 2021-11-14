#include <errno.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

#include <sys/socket.h>

#include <sys/types.h>

#include <string.h>

#include <stddef.h>

#include <sys/un.h>


int main(int argc, char const * argv[]) 
  {
  struct sockaddr_un socketServingaddr, linkingSocket;
  char buffer[BUFSIZ];
  int socketServing, lengthServer, binding;
  int bytesReceived = 0;
  memset(buf, 0, BUFSIZ);
  memset( & socketServingaddr, 0, sizeof(struct sockaddr_un));
  socketServing = socket(AF_UNIX, SOCK_DGRAM, 0);
  if (socketServing == -1) 
  {
    perror("Connection Failed \n");
    exit(2);
  }
  strcpy(socketServingaddr.sun_path, argv[1]);
  socketServingaddr.sun_family = AF_UNIX;
  lengthServer = sizeof(socketServingaddr);
  unlink(argv[1]);
  rc = bind(socketServing, (struct sockaddr * ) & socketServingaddr, lengthServer);
  if (rc == -1) 
  {
    perror("Binding Failed \n");
    close(socketServing);
    exit(1);
  }
  printf("Connected successfully\n");
  while ((bytesReceived = recvfrom(socketServing, buffer, BUFSIZ, 0, (struct sockaddr * ) & linkingSocket, & lengthServer)) > 0) 
  {
    printf("%s\n", buffer);
    buffer[bytesReceived - 1] = '\0';
    int response = system(buffer);
    if (response == 0) strcat(buffer, " returned 0 (Success) ");
    if (response == -1) strcat(buffer, " : returned -1 ");
    if (response > 0) strcat(buffer, " : returned >0 ");
    strcat(buffer, " ==> ");
    strcat(buffer, argv[1]);
    sendto(socketServing, buffer, 1024, 0, (struct sockaddr * ) & linkingSocket, sizeof(struct sockaddr * ));
  }
  /* closing connections */
  close(socketServing);
  return 0;
}