#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <CUnit/CUnit.h>

#include "util.h"
#include "socklib.h"
#include "protocol.h"

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

static CU_TestInfo tests_util[] = {
     {"test_parse_hostport", test_parse_hostport},
     CU_TEST_INFO_NULL,
};

static CU_SuiteInfo suites[] = {
     {"tests_util", NULL, NULL, NULL, NULL, tests_util},
     CU_SUITE_INFO_NULL,
};

void AddTests(void)
{
     assert(NULL != CU_get_registry());
     assert(!CU_is_test_running());

     /* Register suites. */
     if (CU_register_suites(suites) != CUE_SUCCESS) {
          fprintf(stderr, "suite registration failed - %s\n",
                  CU_get_error_msg());
          exit(EXIT_FAILURE);
     }
}

void print_example_results(void)
{
     fprintf(stdout, "Results\n");
}
