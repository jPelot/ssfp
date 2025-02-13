
#if defined(_WIN32)
#ifndef _WIN32_WINNIT
#define _WIN32_WINNIT 0x0600
#endif
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <conio.h>
#include <ws2tcpip.h>

#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

#if defined(_WIN32)
#define ISVALIDSOCKET(s) ((s) != INVALID_SOCKET)
#define CLOSESOCKET(s) closesocket(s)
#define GETSOCKETERRNO() (WSAGetLastError())

#else
#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)
#endif

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


#ifndef socket_h
#define socket_h

SOCKET start_server();
char* server_listen(SOCKET sockfd);
void server_send(char *str);
void server_close(SOCKET socket_listen);

SOCKET start_client(char *ip, char *port);
void client_send(char* message);
void client_close(SOCKET socket);
char *client_read();
char *read_fd(int fd);

#endif // socket_h
