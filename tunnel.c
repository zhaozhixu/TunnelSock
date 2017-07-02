#include "socklib.h"
#include "util.h"
#include "protocol.h"

#define DEFAULT_PORT 2222

extern char ACK_SERVER_READY[];
extern char ACK_SERVER_FAIL[];

static void usage(void)
{
     fprintf(stderr,
             "usage: tunnel host:port localhost:port\n");
     exit(0);
}

int main(int argc, char *argv[])
{
     int daemon_port = DEFAULT_PORT;
     int sfd, lsfd;
     char buf[BUFSIZ];
     int n;
     char new_ack_ready[] = "ready";
     char new_ack_fail[] = "fail";

     if (argc != 3)
          usage();

     char *host, *port, *hostl, *portl;
     parse_hostport(argv[1], &host, &port);
     parse_hostport(argv[2], &hostl, &portl);

     if ((sfd = connect_to_server(host, daemon_port)) == -1) {
          perror("connect host");
          exit(EXIT_FAILURE);
     }
     write(sfd, argv[1], strlen(argv[1])+1);
     n = read(sfd, buf, sizeof(buf));
     if (strncmp(buf, "not bind", n) == 0) {
          fprintf(stderr, "server cannot bind\n");
          exit(EXIT_FAILURE);
     }
     fprintf(stderr, "bind success\n");

     while(1) {                 /* should make parallel */
          n = read(sfd, buf, sizeof(buf));
          fprintf(stderr, "got a request: %s %d\n", buf, n);

          if (is_new_client(buf, n)) {
               fprintf(stderr, "connect webserver\n");
               if ((lsfd = connect_to_server(hostl, atoi(portl))) == -1) {
                    perror("connect hostl");
                    write(sfd, ACK_SERVER_FAIL,
                          strlen(ACK_SERVER_FAIL));
                    exit(EXIT_FAILURE);
               }
               write(sfd, ACK_SERVER_READY, strlen(ACK_SERVER_READY));
               set_nonblock(sfd);
               set_nonblock(lsfd);

               protocol_talk(lsfd, sfd);
               close(lsfd);     /* shutdown? */
               fprintf(stderr, "done webserver\n");
               set_block(sfd);
          }
     }
}
