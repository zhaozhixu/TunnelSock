#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>
#include <CUnit/Basic.h>

#include "CUTestCases.h"

int main(int argc, char *argv[])
{
     setvbuf(stdout, NULL, _IONBF, 0);
     CU_set_error_action(CUEA_IGNORE);

     CU_initialize_registry();
     /* if (CU_initialize_registry()) */
     /*      printf("Initialization of Test Registry failed."); */
     /* else { */
     AddTests();
     CU_set_output_filename("TestAutomated");
     CU_list_tests_to_file();
     CU_automated_run_tests();
     CU_cleanup_registry();
     /* } */
}
