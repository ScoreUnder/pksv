#include <stdlib.h>
#include <check.h>

Suite* uint32_interval_suite(void);

int main(void) {
  int number_failed;
  Suite* s;
  SRunner* sr;

  s = uint32_interval_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_ENV);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
