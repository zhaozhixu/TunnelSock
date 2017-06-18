#include "socklib.h"

int main(int argc, char *argv[])
{
     /* char req[] = "GET / HTTP/1.1\r\n\ */
/* Host: localhost:80\r\n\ */
/* User-Agent: curl/7.47.0\r\n\ */
/* Accept: *\/\*\r\n\r\n"; */
/*      int fd = connect_to_server(argv[1], atoi(argv[2])); */
/*      write(fd, req, strlen(req)); */
/*      char buf[BUFSIZ]; */
/*      int n, i = 0; */
/*      /\* while ((n = read(fd, buf, sizeof(buf))) > 0) { *\/ */
/*      n = read(fd, buf, sizeof(buf)); */
/*      write(1, buf, n); */
/*      printf("%d %d\n", n, ++i); */
/*      write(fd, req, strlen(req)); */
/*      n = read(fd, buf, sizeof(buf)); */
/*      /\* while ((n = read(fd, buf, sizeof(buf))) > 0) { *\/ */
/*      write(1, buf, n); */
/*      printf("%d %d\n", n, ++i) */;
     /* printf("%s", buf); */
     int n;
     char buf[BUFSIZ];
     close(0);
     n = read(0, buf, sizeof(buf));
     printf("%d\n", n);
     exit(0);
}
