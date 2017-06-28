#include "socklib.h"
#include "protocol.h"

extern char REQ_NEW_CLIENT[];
extern char REQ_CLOSE_FD[];
extern char REQ_NEW_FORWARD[];
extern char ACK_SERVER_READY[];
extern char ACK_SERVER_FAIL[];

static int digit_num(int n)
{
     int i;
     for (i = 1; n / 10; i++)
          n /= 10;
     return i;
}

void test_pack(void)
{
     char data[] = "Hello, world!\n";
     char *packed;
     char *unpacked;
     size_t n;

     printf("data length: %ld\n", sizeof(data));

     n = pack_data(data, sizeof(data), &packed);
     printf("%s", packed);
     printf("packed length: %ld\n", n);

     n = unpack_data(packed, &unpacked);
     printf("%s", unpacked);
     printf("unpacked length: %ld\n", n);
}

void test_req_ack(void)
{
     printf("NEW CLIENT: %d\n", is_new_client(REQ_NEW_CLIENT, strlen(REQ_NEW_CLIENT)));
     printf("CLOSE FD: %d\n", is_close_fd(REQ_CLOSE_FD, strlen(REQ_CLOSE_FD)));
     printf("SERVER FAIL: %d\n", is_server_fail(ACK_SERVER_FAIL, strlen(ACK_SERVER_FAIL)));
     printf("SERVER READY: %d\n", is_server_ready(ACK_SERVER_READY, strlen(ACK_SERVER_READY)));
}

int main(int argc, char *argv[])
{
     /* test_pack(); */
     test_req_ack();
}
