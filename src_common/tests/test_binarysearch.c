#include <stdlib.h>
#include <check.h>

#include "binarysearch.h"
#include "binarysearch_u32.h"

START_TEST(test_binary_search_upsert_u32)
{
  struct bsearch_root root;
  bsearch_init_root(&root, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL, NULL);

  // Initial state
  bsearch_upsert_u32(&root, 100, 1234);
  bsearch_upsert_u32(&root, 200, 2345);
  bsearch_upsert_u32(&root, 300, 3456);
  bsearch_upsert_u32(&root, 400, 4567);
  bsearch_upsert_u32(&root, 500, 5678);

  // Insert a few out-of-order
  bsearch_upsert_u32(&root, 340, 1);
  bsearch_upsert_u32(&root, 320, 2);
  bsearch_upsert_u32(&root, 310, 3);
  bsearch_upsert_u32(&root, 330, 4);

  // Overwrite a few
  bsearch_upsert_u32(&root, 100, 12345);
  bsearch_upsert_u32(&root, 500, 999);
  bsearch_upsert_u32(&root, 400, 888);

  // Check final results
  ck_assert_int_eq(bsearch_get_val_u32(&root, 100, 0), 12345);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 200, 0), 2345);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 300, 0), 3456);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 310, 0), 3);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 320, 0), 2);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 330, 0), 4);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 340, 0), 1);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 400, 0), 888);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 500, 0), 999);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 550, 0), 0);
  ck_assert_int_eq(root.size, 9);

  bsearch_deinit_root(&root);
}
END_TEST

START_TEST(test_binary_search_delete_u32)
{
  struct bsearch_root root;
  bsearch_init_root(&root, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL, NULL);

  // Initial state
  bsearch_upsert_u32(&root, 100, 1234);
  bsearch_upsert_u32(&root, 200, 2345);
  bsearch_upsert_u32(&root, 300, 3456);
  bsearch_upsert_u32(&root, 400, 4567);
  bsearch_upsert_u32(&root, 500, 5678);

  ptrdiff_t index;
  // Delete start
  index = bsearch_find_u32(&root, 100);
  ck_assert_int_eq(index, 0);
  bsearch_remove(&root, index);

  // Delete end
  index = bsearch_find_u32(&root, 500);
  ck_assert_int_eq(index, 3);
  bsearch_remove(&root, index);

  // Delete middle
  index = bsearch_find_u32(&root, 300);
  ck_assert_int_eq(index, 1);
  bsearch_remove(&root, index);

  // Check final results
  ck_assert_int_eq(bsearch_get_val_u32(&root, 100, 0), 0);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 200, 0), 2345);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 300, 0), 0);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 400, 0), 4567);
  ck_assert_int_eq(bsearch_get_val_u32(&root, 500, 0), 0);
  ck_assert_int_eq(root.size, 2);

  bsearch_deinit_root(&root);
}
END_TEST

START_TEST(test_binary_search_find_u32)
{
  struct bsearch_root root;
  bsearch_init_root(&root, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL, NULL);

  // Initial state
  bsearch_upsert_u32(&root, 100, 0);
  bsearch_upsert_u32(&root, 200, 0);
  bsearch_upsert_u32(&root, 300, 0);

  // Check exact matches
  ck_assert_int_eq(bsearch_find_u32(&root, 100), 0);
  ck_assert_int_eq(bsearch_find_u32(&root, 200), 1);
  ck_assert_int_eq(bsearch_find_u32(&root, 300), 2);

  // Check insert positions
  ck_assert_int_eq(bsearch_find_u32(&root, 50), -1);
  ck_assert_int_eq(bsearch_find_u32(&root, 150), -2);
  ck_assert_int_eq(bsearch_find_u32(&root, 250), -3);
  ck_assert_int_eq(bsearch_find_u32(&root, 350), -4);

  bsearch_deinit_root(&root);
}
END_TEST

Suite* bsearch_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create("Binary Search");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_binary_search_upsert_u32);
  tcase_add_test(tc_core, test_binary_search_delete_u32);
  tcase_add_test(tc_core, test_binary_search_find_u32);
  suite_add_tcase(s, tc_core);

  return s;
}

int main(void) {
  int number_failed;
  Suite* s;
  SRunner* sr;

  s = bsearch_suite();
  sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
