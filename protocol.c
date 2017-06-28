#include "protocol.h"

#define TMP_BUFSIZ 20

char REQ_NEW_CLIENT[] = "Req: NEW CLIENT\r\n\r\n";
char REQ_CLOSE_FD[] = "Req: CLOSE FD\r\n\r\n";
char REQ_NEW_FORWARD[] = "Req: NEW FORWARD\r\n\r\n";
char ACK_SERVER_READY[] = "Ack: SERVER READY\r\n\r\n";
char ACK_SERVER_FAIL[] = "Ack: SERVER FAIL\r\n\r\n";

static char REQ_DATA_FMT[] =
     "Req: DATA\r\n"
     "Length: %d\r\n\r\n";

static int digit_num(size_t n)
{
     int i;
     for (i = 1; n / 10; i++)
          n /= 10;
     return i;
}

int is_new_client(char *buf, size_t n)
{
     return strncmp(buf, REQ_NEW_CLIENT, n) == 0;
}

int is_close_fd(char *buf, size_t n)
{
     return strncmp(buf, REQ_CLOSE_FD, n) == 0;
}

int is_server_ready(char *buf, size_t n)
{
     return strncmp(buf, ACK_SERVER_READY, n) == 0;
}

int is_server_fail(char *buf, size_t n)
{
     return strncmp(buf, ACK_SERVER_FAIL, n) == 0;
}

size_t pack_data(char *s, size_t n, char **t)
{
     size_t h_length;

     *t = malloc(sizeof(REQ_DATA_FMT) - 2 + digit_num(n) + n);
     if (*t == NULL) {
          perror("pack_data: error malloc");
          exit(EXIT_FAILURE);
     }

     sprintf(*t, REQ_DATA_FMT, n);
     h_length = strlen(*t);
     memmove(*t + h_length, s, n);

     return h_length + n;
}

size_t unpack_data(char *s, char **t)
{
     char req[TMP_BUFSIZ], req_arg[TMP_BUFSIZ];
     char length[TMP_BUFSIZ], length_arg[TMP_BUFSIZ];
     char *data;
     size_t n = 0;

     sscanf(s, "%s%s%s%s", req, req_arg, length, length_arg);

     if (strncmp(req, "Req:", 4) || strncmp(req_arg, "DATA", 4)
         || strncmp(length, "Length:", 7)) {
          fprintf(stderr, "unpack_data: wrong format\n");
          return -1;
     }

     n = strtoul(length_arg, NULL, 10);
     if (n == 0)
          return 0;

     *t = malloc(n);
     if (*t == NULL) {
          perror("unpack_data: malloc error");
          exit(EXIT_FAILURE);
     }

     data = strstr(s, "\r\n\r\n") + 4;
     memmove(*t, data, n);
     return n;
}

void protocol_talk(int fd_raw, int fd_pack)
{
     int maxfdp1, n;
     char buf[BUFSIZ];
     char *buf_pack, *buf_raw;
     fd_set fds;

     maxfdp1 = 1 + (fd_raw > fd_pack ? fd_raw : fd_pack);
     FD_ZERO(&fds);

     while (1) {
          FD_SET(fd_raw, &fds);
          FD_SET(fd_pack, &fds);
          select(maxfdp1, &fds, NULL, NULL, NULL);

          if (FD_ISSET(fd_raw, &fds)) {
               if ((n = read(fd_raw, buf, sizeof(buf))) == 0) {
                    write(fd_pack, REQ_CLOSE_FD,
                          sizeof(REQ_CLOSE_FD));
                    write(2, REQ_CLOSE_FD,
                          sizeof(REQ_CLOSE_FD));
                    return;
               }
               n = pack_data(buf, n, &buf_pack);
               write(fd_pack, buf_pack, n);
               write(2, buf_pack, n);
               free(buf_pack);
          }
          if (FD_ISSET(fd_pack, &fds)) {
               if ((n = read(fd_pack, buf, sizeof(buf))) == 0)
                    return;
               if (is_close_fd(buf, n))
                    return;     /* more msg to fd_raw? */
               n = unpack_data(buf, &buf_raw);
               write(fd_raw, buf_raw, n);
               write(2, buf_raw, n);
               free(buf_raw);
          }
     }
}
