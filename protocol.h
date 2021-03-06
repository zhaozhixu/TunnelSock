#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>

int digit_num(size_t n);
int is_new_client(char *buf, size_t n);
int is_new_forward(char *buf, size_t n);
int is_close_fd(char *buf, size_t n);
int is_server_ready(char *buf, size_t n);
int is_server_fail(char *buf, size_t n);
size_t pack_data(char *s, size_t n, char **t);
ssize_t unpack_data(char *s, char **t);
void protocol_talk(int fd_raw, int fd_pack);
