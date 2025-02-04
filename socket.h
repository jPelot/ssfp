#ifndef socket_h
#define socket_h

int start_server();
int start_client(char *ip, int port);
int server_listen(int sockfd);
void client_send(int sockfd, char* message);
char *read_fd(int fd);

#endif // socket_h
