#include "socklib.h"

static int make_server_socket_q(int portnum, int backlog);

int make_server_socket(int portnum)
{
     return make_server_socket_q(portnum, BACKLOG);
}

int connect_to_server(char* host,int portnum)
{
     struct addrinfo hints;
     struct addrinfo *result, *rp;
     char servname[SERVLEN];
     int sfd, s;

     memset(&hints, 0, sizeof(hints));
     hints.ai_family = AF_UNSPEC;
     hints.ai_socktype = SOCK_STREAM;
     hints.ai_flags = 0;
     hints.ai_protocol = 0;

     sprintf(servname, "%d", portnum);
     s = getaddrinfo(host, servname, &hints, &result);
     if (s != 0) {
          fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
          /* exit(EXIT_FAILURE); */
          return -1;
     }

     for (rp = result; rp != NULL; rp = rp->ai_next) {
          sfd = socket(rp->ai_family, rp->ai_socktype,
                       rp->ai_protocol);
          if (sfd == -1)
               continue;

          if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
               break;           /* success */

          close(sfd);
     }

     if (rp == NULL) {          /* no address succeeded */
          fprintf(stderr, "Could not connect\n");
          /* exit(EXIT_FAILURE); */
          return -1;
     }

     freeaddrinfo(result);

     return sfd;
}

static int make_server_socket_q(int portnum, int backlog)
{
     struct addrinfo hints;
     struct addrinfo *result, *rp;
     char hostname[HOSTLEN];
     char servname[SERVLEN];
     int sfd, s;

     memset(&hints, 0, sizeof(hints));
     hints.ai_family = AF_INET;
     hints.ai_socktype = SOCK_STREAM;
     hints.ai_flags = AI_PASSIVE;
     hints.ai_protocol = 0;
     hints.ai_canonname = NULL;
     hints.ai_addr = NULL;
     hints.ai_next = NULL;

     sprintf(servname, "%d", portnum);
     s = getaddrinfo(NULL, servname, &hints, &result);
     if (s != 0) {
          fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
          return -1;
     }

     for (rp = result; rp != NULL; rp = rp->ai_next) {
          sfd = socket(rp->ai_family, rp->ai_socktype,
                       rp->ai_protocol);
          if (sfd == -1)
               continue;

          if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
               break;           /* success */

          close(sfd);
     }

     if (rp == NULL) {          /* No address succeeded */
          fprintf(stderr, "Could not bind\n");
          return -1;
     }

     freeaddrinfo(result);

     if (listen(sfd, backlog) != 0)
          return -1;
     return sfd;
}
