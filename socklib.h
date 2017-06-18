#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#define HOSTLEN 256
#define SERVLEN 256
#define BACKLOG 1

int make_server_socket(int portnum);
int connect_to_server(char *host, int portnum);
