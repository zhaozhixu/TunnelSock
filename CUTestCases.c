#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include <CUnit/CUnit.h>

#include "util.h"
#include "socklib.h"
#include "protocol.h"
#include "CUTestCases.h"

static void test_parse_hostport(void)
{
     char *hostport[5];
     char *hostp, *portp;
     int i;

     hostport[0] = "localhost:1234";
     hostport[1] = "localhost:";
     hostport[2] = ":1234";
     hostport[3] = ":";
     hostport[4] = "nosense";

     for (i = 0; i < 5; i++) {
          parse_hostport(hostport[i], &hostp, &portp);
          switch (i) {
          case 0:
               CU_ASSERT_STRING_EQUAL(hostp, "localhost");
               CU_ASSERT_STRING_EQUAL(portp, "1234");
               break;
          case 1:
               CU_ASSERT_STRING_EQUAL(hostp, "localhost");
               CU_ASSERT_STRING_EQUAL(portp, "");
               break;
          case 2:
               CU_ASSERT_STRING_EQUAL(hostp, "");
               CU_ASSERT_STRING_EQUAL(portp, "1234");
               break;
          case 3:
               CU_ASSERT_STRING_EQUAL(hostp, "");
               CU_ASSERT_STRING_EQUAL(portp, "");
               break;
          case 4:
               CU_ASSERT_STRING_EQUAL(hostp, "");
               CU_ASSERT_STRING_EQUAL(portp, "");
               break;
          }
          free(hostp);
          free(portp);
     }
}

static void test_digit_num(void)
{
     int bit, width;
     size_t integer;

     CU_ASSERT_EQUAL(digit_num(0), 1);
     CU_ASSERT_EQUAL(digit_num(1), 1);
     CU_ASSERT_EQUAL(digit_num(20), 2);

     bit = sizeof(int*) * 8;
     if (bit == 32) {
          integer = 0xFFFFFFFF;
          width = 10;
     }
     else {
          integer = 0xFFFFFFFFFFFFFFFF;
          width = 20;
     }
     CU_ASSERT_EQUAL(digit_num(integer), width);
     CU_ASSERT_EQUAL(digit_num(integer-1), width);
     /* printf("%lu->%d\n", integer, digit_num(integer)); */
}

static void test_is_new_client(void)
{
     char req_new_client[] = "Req: NEW CLIENT\r\n\r\n";
     char req_close_fd[] = "Req: CLOSE FD\r\n\r\n";
     char ack_server_ready[] = "Ack: SERVER READY\r\n\r\n";
     char req_wrong_newline[] = "Req: NEW CLIENT\n\n";
     char nonsense[] = "nonsense";
     char empty[] = "";

     CU_ASSERT_TRUE(is_new_client(req_new_client,
                                  strlen(req_new_client)));
     CU_ASSERT_FALSE(is_new_client(req_close_fd,
                                   strlen(req_close_fd)));
     CU_ASSERT_FALSE(is_new_client(ack_server_ready,
                                   strlen(ack_server_ready)));
     CU_ASSERT_FALSE(is_new_client(req_wrong_newline,
                                   strlen(req_wrong_newline)));
     CU_ASSERT_FALSE(is_new_client(nonsense, strlen(nonsense)));
     CU_ASSERT_FALSE(is_new_client(empty, strlen(empty)));

}

static void test_is_new_forward(void)
{
     char req_new_forward[] = "Req: NEW FORWARD\r\n\r\n";
     char req_close_fd[] = "Req: CLOSE FD\r\n\r\n";
     char ack_server_ready[] = "Ack: SERVER READY\r\n\r\n";
     char req_wrong_newline[] = "Req: NEW FORWARD\n\n";
     char nonsense[] = "nonsense";
     char empty[] = "";

     CU_ASSERT_TRUE(is_new_forward(req_new_forward,
                                  strlen(req_new_forward)));
     CU_ASSERT_FALSE(is_new_forward(req_close_fd,
                                   strlen(req_close_fd)));
     CU_ASSERT_FALSE(is_new_forward(ack_server_ready,
                                   strlen(ack_server_ready)));
     CU_ASSERT_FALSE(is_new_forward(req_wrong_newline,
                                   strlen(req_wrong_newline)));
     CU_ASSERT_FALSE(is_new_forward(nonsense, strlen(nonsense)));
     CU_ASSERT_FALSE(is_new_forward(empty, strlen(empty)));
}

static void test_is_close_fd(void)
{
     char req_close_fd[] = "Req: CLOSE FD\r\n\r\n";
     char req_new_forward[] = "Req: NEW FORWARD\r\n\r\n";
     char ack_server_ready[] = "Ack: SERVER READY\r\n\r\n";
     char req_wrong_newline[] = "Req: CLOSE FD\n\n";
     char nonsense[] = "nonsense";
     char empty[] = "";

     CU_ASSERT_TRUE(is_close_fd(req_close_fd,
                                    strlen(req_close_fd)));
     CU_ASSERT_FALSE(is_close_fd(req_new_forward,
                                strlen(req_new_forward)));
     CU_ASSERT_FALSE(is_close_fd(ack_server_ready,
                                    strlen(ack_server_ready)));
     CU_ASSERT_FALSE(is_close_fd(req_wrong_newline,
                                    strlen(req_wrong_newline)));
     CU_ASSERT_FALSE(is_close_fd(nonsense, strlen(nonsense)));
     CU_ASSERT_FALSE(is_close_fd(empty, strlen(empty)));
}

static void test_is_server_ready(void)
{
     char ack_server_ready[] = "Ack: SERVER READY\r\n\r\n";
     char ack_server_fail[] = "Ack: SERVER FAIL\r\n\r\n";
     char req_new_forward[] = "Req: NEW FORWARD\r\n\r\n";
     char ack_wrong_newline[] = "Ack: SERVER READY\n\n";
     char nonsense[] = "nonsense";
     char empty[] = "";

     CU_ASSERT_TRUE(is_server_ready(ack_server_ready,
                                strlen(ack_server_ready)));
     CU_ASSERT_FALSE(is_server_ready(ack_server_fail,
                                 strlen(ack_server_fail)));
     CU_ASSERT_FALSE(is_server_ready(req_new_forward,
                                     strlen(req_new_forward)));
     CU_ASSERT_FALSE(is_server_ready(ack_wrong_newline,
                                 strlen(ack_wrong_newline)));
     CU_ASSERT_FALSE(is_server_ready(nonsense, strlen(nonsense)));
     CU_ASSERT_FALSE(is_server_ready(empty, strlen(empty)));
}

static void test_is_server_fail(void)
{
     char ack_server_fail[] = "Ack: SERVER FAIL\r\n\r\n";
     char ack_server_ready[] = "Ack: SERVER READY\r\n\r\n";
     char req_new_forward[] = "Req: NEW FORWARD\r\n\r\n";
     char ack_wrong_newline[] = "Ack: SERVER FAIL\n\n";
     char nonsense[] = "nonsense";
     char empty[] = "";

     CU_ASSERT_TRUE(is_server_fail(ack_server_fail,
                                     strlen(ack_server_fail)));
     CU_ASSERT_FALSE(is_server_fail(ack_server_ready,
                                   strlen(ack_server_ready)));
     CU_ASSERT_FALSE(is_server_fail(req_new_forward,
                                     strlen(req_new_forward)));
     CU_ASSERT_FALSE(is_server_fail(ack_wrong_newline,
                                     strlen(ack_wrong_newline)));
     CU_ASSERT_FALSE(is_server_fail(nonsense, strlen(nonsense)));
     CU_ASSERT_FALSE(is_server_fail(empty, strlen(empty)));
}

static void test_pack_unpack_data(void)
{
     char raw_string[] = "hello world!\n"
          "It's me\r\n\r\n";
     char packed_string[] = "Req: DATA\r\n"
          "Length: 25\r\n\r\n"
          "hello world!\n"
          "It's me\r\n\r\n";
     char empty[] = "";
     char packed_empty[] = "Req: DATA\r\n"
          "Length: 1\r\n\r\n";
     char wrong_format[] = "Req: NEW CLIENT\r\n\r\n";
     char *packed, *unpacked;
     char buf[BUFSIZ];
     ssize_t n, fn;
     int fd;

     n = pack_data(raw_string, strlen(raw_string)+1, &packed);
     CU_ASSERT_EQUAL(n, sizeof(packed_string));
     CU_ASSERT_STRING_EQUAL(packed_string, packed);
     unpack_data(packed, &unpacked);
     CU_ASSERT_STRING_EQUAL(raw_string, unpacked);
     free(packed);
     free(unpacked);

     n = pack_data(empty, strlen(empty)+1, &packed);
     CU_ASSERT_EQUAL(n, sizeof(packed_empty));
     CU_ASSERT_STRING_EQUAL(packed_empty, packed);
     unpack_data(packed, &unpacked);
     CU_ASSERT_STRING_EQUAL(empty, unpacked);
     free(packed);
     free(unpacked);

     /* binary data test */
     fd = open("test", O_RDONLY);
     fn = read(fd, buf, BUFSIZ);
     n = pack_data(buf, fn, &packed);
     CU_ASSERT_EQUAL(fn+sizeof(packed_empty)-2+digit_num(fn), n);
     unpack_data(packed, &unpacked);
     CU_ASSERT_NSTRING_EQUAL(buf, unpacked, fn);
     close(fd);
     free(packed);
     free(unpacked);

     n = unpack_data(wrong_format, &unpacked);
     CU_ASSERT_EQUAL(n, -1);
     n = unpack_data(empty, &unpacked);
     CU_ASSERT_EQUAL(n, -1);
}

static CU_TestInfo tests_util[] = {
     {"test_parse_hostport", test_parse_hostport},
     CU_TEST_INFO_NULL,
};

static CU_TestInfo tests_protocol[] = {
     {"test_digit_num", test_digit_num},
     {"test_is_new_client", test_is_new_client},
     {"test_is_new_forward", test_is_new_forward},
     {"test_is_close_fd", test_is_close_fd},
     {"test_is_server_ready", test_is_server_ready},
     {"test_is_server_fail", test_is_server_fail},
     {"test_pack_unpack_data", test_pack_unpack_data},
     CU_TEST_INFO_NULL,
};

static CU_SuiteInfo suites[] = {
     {"tests_util", NULL, NULL, NULL, NULL, tests_util},
     {"tests_protocol", NULL, NULL, NULL, NULL, tests_protocol},
     CU_SUITE_INFO_NULL,
};

void AddTests(void)
{
     assert(NULL != CU_get_registry());
     assert(!CU_is_test_running());

     /* Register suites. */
     CU_register_suites(suites);
     /* if (CU_register_suites(suites) != CUE_SUCCESS) { */
     /*      fprintf(stderr, "suite registration failed - %s\n", */
     /*              CU_get_error_msg()); */
     /*      exit(EXIT_FAILURE); */
     /* } */
}

/* void print_example_results(void) */
/* { */
/*      fprintf(stdout, "Results\n"); */
/* } */
