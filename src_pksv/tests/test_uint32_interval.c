#include <check.h>

#include "binarysearch_u32.h"
#include "../uint32_interval.h"

static struct bsearch_root root;

void setup(void) {
  uint32_interval_init_bsearch_root(&root);

  // starting at 1 so still room to move left
  uint32_interval_add(&root, 1, 10);
  uint32_interval_add(&root, 20, 30);

  ck_assert_uint_eq(root.size, 2);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 10);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 20);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 30);

  uint32_interval_add(&root, 40, 50);
  uint32_interval_add(&root, 60, 70);
  uint32_interval_add(&root, 80, 90);

  ck_assert_uint_eq(root.size, 5);
}

void teardown(void) { bsearch_deinit_root(&root); }

START_TEST(test_intervals_add) {
  uint32_interval_add(&root, 14, 16);

  ck_assert_uint_eq(root.size, 6);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 10);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 14);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 16);
  ck_assert_uint_eq(bsearch_key_u32(&root, 2), 20);
  ck_assert_uint_eq(bsearch_val_u32(&root, 2), 30);
}
END_TEST

START_TEST(test_intervals_extend) {
  // Overlap with first interval
  uint32_interval_add(&root, 5, 15);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 15);

  // Overlap with the second interval, starting at the same point
  uint32_interval_add(&root, 20, 25);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 20);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 30);

  // Same start, but end is after the second interval
  uint32_interval_add(&root, 20, 35);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 20);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 35);

  // Extend the 3rd interval left a little
  uint32_interval_add(&root, 39, 45);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 2), 39);
  ck_assert_uint_eq(bsearch_val_u32(&root, 2), 50);

  // Extend the 3rd interval right a little
  uint32_interval_add(&root, 50, 51);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 2), 39);
  ck_assert_uint_eq(bsearch_val_u32(&root, 2), 51);

  // Completely overlap the 4th interval
  uint32_interval_add(&root, 57, 73);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 3), 57);
  ck_assert_uint_eq(bsearch_val_u32(&root, 3), 73);

  // Extend the 5th interval left, with the same end point
  uint32_interval_add(&root, 79, 90);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 4), 79);
  ck_assert_uint_eq(bsearch_val_u32(&root, 4), 90);

  // Extend the 5th interval left again, ending at its start
  uint32_interval_add(&root, 78, 79);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 4), 78);
  ck_assert_uint_eq(bsearch_val_u32(&root, 4), 90);
}
END_TEST

START_TEST(test_intervals_merge) {
  // Merge 1st and 2nd intervals exactly
  uint32_interval_add(&root, 10, 20);

  ck_assert_uint_eq(root.size, 4);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 30);

  // Merge 2nd and 3rd intervals sloppily
  uint32_interval_add(&root, 45, 65);

  ck_assert_uint_eq(root.size, 3);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 40);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 70);

  // Merge 2nd and 3rd intervals with total overlap and more
  uint32_interval_add(&root, 35, 100);

  ck_assert_uint_eq(root.size, 2);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 35);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 100);
}
END_TEST

START_TEST(test_intervals_merge_all_1) {
  uint32_interval_add(&root, 10, 80);

  ck_assert_uint_eq(root.size, 1);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 90);
}
END_TEST

START_TEST(test_intervals_merge_all_2) {
  uint32_interval_add(&root, 1, 90);

  ck_assert_uint_eq(root.size, 1);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 90);
}
END_TEST

START_TEST(test_intervals_merge_all_3) {
  uint32_interval_add(&root, 0, 100);

  ck_assert_uint_eq(root.size, 1);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 0);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 100);
}
END_TEST

START_TEST(test_intervals_redundant) {
  uint32_interval_add(&root, 1, 10);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 10);

  uint32_interval_add(&root, 2, 9);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 10);

  uint32_interval_add(&root, 80, 85);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 4), 80);
  ck_assert_uint_eq(bsearch_val_u32(&root, 4), 90);

  uint32_interval_add(&root, 81, 90);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 4), 80);
  ck_assert_uint_eq(bsearch_val_u32(&root, 4), 90);
}
END_TEST

START_TEST(test_intervals_empty) {
  uint32_interval_add(&root, 6, 6);

  ck_assert_uint_eq(root.size, 5);

  uint32_interval_add(&root, 999, 999);

  ck_assert_uint_eq(root.size, 5);
}
END_TEST

START_TEST(test_intervals_delete_simple) {
  // Delete first
  uint32_interval_remove(&root, 1, 10);

  ck_assert_uint_eq(root.size, 4);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 20);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 30);

  // Delete last
  uint32_interval_remove(&root, 80, 90);

  ck_assert_uint_eq(root.size, 3);

  // Delete middle
  uint32_interval_remove(&root, 40, 50);

  ck_assert_uint_eq(root.size, 2);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 20);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 30);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 60);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 70);
}
END_TEST

START_TEST(test_intervals_delete_split) {
  uint32_interval_remove(&root, 4, 6);

  ck_assert_uint_eq(root.size, 6);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 4);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 6);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 10);

  uint32_interval_remove(&root, 85, 87);

  ck_assert_uint_eq(root.size, 7);
  ck_assert_uint_eq(bsearch_key_u32(&root, 5), 80);
  ck_assert_uint_eq(bsearch_val_u32(&root, 5), 85);
  ck_assert_uint_eq(bsearch_key_u32(&root, 6), 87);
  ck_assert_uint_eq(bsearch_val_u32(&root, 6), 90);

  uint32_interval_remove(&root, 45, 48);

  ck_assert_uint_eq(root.size, 8);
  ck_assert_uint_eq(bsearch_key_u32(&root, 3), 40);
  ck_assert_uint_eq(bsearch_val_u32(&root, 3), 45);
  ck_assert_uint_eq(bsearch_key_u32(&root, 4), 48);
  ck_assert_uint_eq(bsearch_val_u32(&root, 4), 50);
}
END_TEST

START_TEST(test_intervals_delete_trim_end) {
  uint32_interval_remove(&root, 5, 10);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 5);

  uint32_interval_remove(&root, 85, 95);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 4), 80);
  ck_assert_uint_eq(bsearch_val_u32(&root, 4), 85);

  uint32_interval_remove(&root, 45, 60);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 2), 40);
  ck_assert_uint_eq(bsearch_val_u32(&root, 2), 45);
}
END_TEST

START_TEST(test_intervals_delete_trim_start) {
  uint32_interval_remove(&root, 1, 5);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 5);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 10);

  uint32_interval_remove(&root, 70, 85);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 4), 85);
  ck_assert_uint_eq(bsearch_val_u32(&root, 4), 90);

  uint32_interval_remove(&root, 35, 45);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 2), 45);
  ck_assert_uint_eq(bsearch_val_u32(&root, 2), 50);
}
END_TEST

START_TEST(test_intervals_delete_empty) {
  uint32_interval_remove(&root, 3, 3);

  ck_assert_uint_eq(root.size, 5);

  uint32_interval_remove(&root, 999, 999);

  ck_assert_uint_eq(root.size, 5);
}
END_TEST

START_TEST(test_intervals_delete_trim_both) {
  uint32_interval_remove(&root, 5, 25);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 25);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 30);

  uint32_interval_remove(&root, 65, 85);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 3), 60);
  ck_assert_uint_eq(bsearch_val_u32(&root, 3), 65);
  ck_assert_uint_eq(bsearch_key_u32(&root, 4), 85);
  ck_assert_uint_eq(bsearch_val_u32(&root, 4), 90);
}
END_TEST

START_TEST(test_intervals_delete_overlap_1) {
  // Delete overlapping a whole interval
  uint32_interval_remove(&root, 35, 55);

  ck_assert_uint_eq(root.size, 4);
  ck_assert_uint_eq(bsearch_key_u32(&root, 2), 60);
  ck_assert_uint_eq(bsearch_val_u32(&root, 2), 70);

  // Same but overlapping another's start
  uint32_interval_remove(&root, 35, 85);

  ck_assert_uint_eq(root.size, 3);
  ck_assert_uint_eq(bsearch_key_u32(&root, 2), 85);
  ck_assert_uint_eq(bsearch_val_u32(&root, 2), 90);

  // Same but overlapping another's end
  uint32_interval_remove(&root, 5, 35);

  ck_assert_uint_eq(root.size, 2);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 85);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 90);
}
END_TEST

START_TEST(test_intervals_delete_overlap_2) {
  // Delete overlapping multiple intervals
  uint32_interval_remove(&root, 35, 1000);

  ck_assert_uint_eq(root.size, 2);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 10);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 20);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 30);
}
END_TEST

START_TEST(test_intervals_delete_overlap_3) {
  // Delete overlapping multiple intervals
  uint32_interval_remove(&root, 0, 55);

  ck_assert_uint_eq(root.size, 2);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 60);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 70);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 80);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 90);
}
END_TEST

START_TEST(test_intervals_delete_overlap_4) {
  // Delete overlapping multiple intervals
  uint32_interval_remove(&root, 0, 1000);

  ck_assert_uint_eq(root.size, 0);
}
END_TEST

START_TEST(test_intervals_delete_overlap_5) {
  // Delete overlapping multiple intervals
  uint32_interval_remove(&root, 1, 90);

  ck_assert_uint_eq(root.size, 0);
}

START_TEST(test_intervals_delete_overlap_6) {
  // Delete overlapping a single interval
  uint32_interval_remove(&root, 0, 11);

  ck_assert_uint_eq(root.size, 4);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 20);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 30);

  uint32_interval_remove(&root, 75, 100);

  ck_assert_uint_eq(root.size, 3);
  ck_assert_uint_eq(bsearch_key_u32(&root, 2), 60);
  ck_assert_uint_eq(bsearch_val_u32(&root, 2), 70);
}

START_TEST(test_intervals_delete_redundant) {
  // Delete overlapping multiple intervals
  uint32_interval_remove(&root, 10, 20);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 10);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 20);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 30);

  uint32_interval_remove(&root, 15, 19);

  ck_assert_uint_eq(root.size, 5);
  ck_assert_uint_eq(bsearch_key_u32(&root, 0), 1);
  ck_assert_uint_eq(bsearch_val_u32(&root, 0), 10);
  ck_assert_uint_eq(bsearch_key_u32(&root, 1), 20);
  ck_assert_uint_eq(bsearch_val_u32(&root, 1), 30);
}

Suite *uint32_interval_suite(void) {
  Suite *s = suite_create("uint32_interval");
  TCase *tc_core = tcase_create("Core");

  tcase_add_checked_fixture(tc_core, setup, teardown);
  tcase_add_test(tc_core, test_intervals_add);
  tcase_add_test(tc_core, test_intervals_extend);
  tcase_add_test(tc_core, test_intervals_merge);
  tcase_add_test(tc_core, test_intervals_merge_all_1);
  tcase_add_test(tc_core, test_intervals_merge_all_2);
  tcase_add_test(tc_core, test_intervals_merge_all_3);
  tcase_add_test(tc_core, test_intervals_redundant);
  tcase_add_test(tc_core, test_intervals_empty);
  tcase_add_test(tc_core, test_intervals_delete_simple);
  tcase_add_test(tc_core, test_intervals_delete_split);
  tcase_add_test(tc_core, test_intervals_delete_trim_end);
  tcase_add_test(tc_core, test_intervals_delete_trim_start);
  tcase_add_test(tc_core, test_intervals_delete_empty);
  tcase_add_test(tc_core, test_intervals_delete_trim_both);
  tcase_add_test(tc_core, test_intervals_delete_overlap_1);
  tcase_add_test(tc_core, test_intervals_delete_overlap_2);
  tcase_add_test(tc_core, test_intervals_delete_overlap_3);
  tcase_add_test(tc_core, test_intervals_delete_overlap_4);
  tcase_add_test(tc_core, test_intervals_delete_overlap_5);
  tcase_add_test(tc_core, test_intervals_delete_overlap_6);
  tcase_add_test(tc_core, test_intervals_delete_redundant);
  suite_add_tcase(s, tc_core);

  return s;
}
