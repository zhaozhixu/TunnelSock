#include "socklib.h"

void parse_hostport(char *hostport, char **hostp, char **portp);
void set_nonblock(int fd);
void set_block(int fd);
void socket_talk(int fd1, int fd2);
