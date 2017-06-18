#include "socklib.h"
#include "util.h"

#define DEFAULT_PORT 2222

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

     while(1) {                 /* should make parallel */
          n = read(sfd, buf, sizeof(buf));
          fprintf(stderr, "got a request: %s %d\n", buf, n);

          if (strncmp(buf, "new request", n) == 0) {
               fprintf(stderr, "connect webserver\n");
               if ((lsfd = connect_to_server(hostl, atoi(portl)))
                   == -1) {
                    perror("connect hostl");
                    write(sfd, new_ack_fail, strlen(new_ack_fail)+1);
                    exit(EXIT_FAILURE);
               }
               write(sfd, new_ack_ready, strlen(new_ack_ready)+1);
               set_nonblock(sfd);
               set_nonblock(lsfd);

               socket_talk(sfd, lsfd);
               close(lsfd);
               fprintf(stderr, "done webserver\n");
               set_block(sfd);
          }
     }
}
