#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>

void parse_hostport(char *hostport, char **hostp, char **portp)
{
     size_t n;

     n = strcspn(hostport, ":");
     if (n == strlen(hostport)) {
          *hostp = (char *)malloc(2);
          *portp = (char *)malloc(2);
          strcpy(*hostp, "");
          strcpy(*portp, "");
          return;
     }

     *hostp = (char *)malloc(n + 1);
     strncpy(*hostp, hostport, n);
     (*hostp)[n] = '\0';

     *portp = (char *)malloc(strlen(hostport + n + 2));
     strcpy(*portp, hostport + n + 1);
}


void set_nonblock(int fd) {
     int flag;
     flag = fcntl(fd, F_GETFL);
     flag |= O_NONBLOCK;
     if (fcntl(fd, F_SETFL, flag) == -1) {
          perror("setting O_NONBLOCK");
          exit(EXIT_FAILURE);
     }
}

void set_block(int fd) {
     int flag;
     flag = fcntl(fd, F_GETFL);
     flag &= ~O_NONBLOCK;
     if (fcntl(fd, F_SETFL, flag) == -1) {
          perror("unsetting O_NONBLOCK");
          exit(EXIT_FAILURE);
     }
}

/* void socket_talk(int fd1, int fd2) { */
/*      int maxfdp1, n, eof1, eof2; */
/*      char buf[BUFSIZ]; */
/*      fd_set fds; */

/*      eof1 = eof2 = 0; */
/*      maxfdp1 = 1 + (fd1 > fd2 ? fd1 : fd2); */
/*      FD_ZERO(&fds); */

/*      while (1) { */
/*           if (eof1 == 0) */
/*                FD_SET(fd1, &fds); */
/*           if (eof2 == 0) */
/*                FD_SET(fd2, &fds); */
/*           select(maxfdp1, &fds, NULL, NULL, NULL); */
/*           fprintf(stderr, "a\n"); */

/*           if (FD_ISSET(fd1, &fds)) { */
/*                fprintf(stderr, "b\n"); */
/*                if ((n = read(fd1, buf, sizeof(buf))) == 0) { */
/*                     /\* if (eof2 == 1) *\/ */
/*                     /\*      return; *\/ */
/*                     /\* eof1 = 1; *\/ */
/*                     /\* /\\* shutdown(fd2, SHUT_WR); *\\/ *\/ */
/*                     /\* FD_CLR(fd1, &fds); *\/ */
/*                     /\* continue; *\/ */
/*                     return; */
/*                } */
/*                write(fd2, buf, n); */
/*                write(1, buf, n); */
/*           } */
/*           if (FD_ISSET(fd2, &fds)) { */
/*                fprintf(stderr, "c\n"); */
/*                if ((n = read(fd2, buf, sizeof(buf))) == 0) { */
/*                     /\* if (eof1 == 1) *\/ */
/*                     /\*      return; *\/ */
/*                     /\* eof2 = 1; *\/ */
/*                     /\* /\\* shutdown(fd1, SHUT_WR); *\\/ *\/ */
/*                     /\* FD_CLR(fd2, &fds); *\/ */
/*                     /\* continue; *\/ */
/*                     return; */
/*                } */
/*                write(fd1, buf, n); */
/*                write(1, buf, n); */
/*           } */
/*      } */
/* } */
