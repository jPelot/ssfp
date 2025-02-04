#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <unistd.h> // read(), write(), close()

#include "socket.h"

#define MAX 800
#define PORT 8080

int
start_server()
{
  int sockfd;
  struct sockaddr_in servaddr;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creating failed...\n");
    exit(0);
  }
  printf("Socket successfully created...\n");

  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) {
    printf("Socket bind failed...\n");
    exit(0);
  }
  printf("Socket successfully binded...\n");
  
  return sockfd;
}

int
start_client(char* ip, int port)
{
  int sockfd;
  struct sockaddr_in servaddr;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creating failed...\n");
    exit(0);
  }
  printf("Socket successfully created...\n");

  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(ip); 
  servaddr.sin_port = htons(port);

  if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0) {
    printf("Connection with the server failed...\n");
    exit(0);
  }
  printf("Connected to the server...\n");
  
  return sockfd;
}

int
server_listen(int sockfd)
{
  int len, connfd;
  struct sockaddr_in cli;

  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed... %s\n", strerror(errno));
    return -1;
  }
  printf("Server listening...\n");

  len = sizeof(cli);

  // Accept the data packet from client and verification
  connfd = accept(sockfd, (struct sockaddr*)&cli, (socklen_t*)&len);

  if (connfd < 0) {
    printf("Server accept failed...\n");
    return -1;
  }
  printf("Server accept the client...\n");

  return connfd;
}

void
client_send(int sockfd, char* message) {
  write(sockfd, message, strlen(message));
}

char*
read_fd(int fd)
{
  char *buff = malloc(MAX);
  char *buffptr = buff;
  int bytes_read;
  int total_bytes = 0;
  while((bytes_read = read(fd, buffptr, MAX)) > 0) {
    total_bytes += bytes_read;
    buffptr += bytes_read;
  }
  buff[total_bytes] = '\0';
  //printf("From client:\n %s", buff);
  return buff;
}
