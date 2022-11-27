#include <stdlib.h>
#include <check.h>

#include "binarysearch.h"
#include "binarysearch_u32.h"
#include "textutil.h"

START_TEST(test_binary_search_upsert_u32) {
  struct bsearch_root root;
  bsearch_init_root(&root, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL,
                    NULL);

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

START_TEST(test_binary_search_delete_u32) {
  struct bsearch_root root;
  bsearch_init_root(&root, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL,
                    NULL);

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

START_TEST(test_binary_search_find_u32) {
  struct bsearch_root root;
  bsearch_init_root(&root, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL,
                    NULL);

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

START_TEST(test_binary_search_string_keys) {
  struct bsearch_root root;
  bsearch_init_root(&root, bsearch_key_strcmp, bsearch_key_strdup, free, NULL);

  bsearch_upsert(&root, "cowabunga", "dude");
  // Insert before, at start
  bsearch_upsert(&root, "cow", "moo");
  // Insert after, at end
  bsearch_upsert(&root, "frog", "ribbit");
  // Insert in middle
  bsearch_upsert(&root, "dog", "woof");

  // Check final results
  ck_assert_str_eq(bsearch_get_val(&root, "cow", NULL), "moo");
  ck_assert_str_eq(bsearch_get_val(&root, "cowabunga", NULL), "dude");
  ck_assert_str_eq(bsearch_get_val(&root, "dog", NULL), "woof");
  ck_assert_str_eq(bsearch_get_val(&root, "frog", NULL), "ribbit");
  ck_assert_int_eq(root.size, 4);

  // Check order
  ck_assert_int_eq(bsearch_find(&root, "cow"), 0);
  ck_assert_int_eq(bsearch_find(&root, "cowabunga"), 1);
  ck_assert_int_eq(bsearch_find(&root, "dog"), 2);
  ck_assert_int_eq(bsearch_find(&root, "frog"), 3);

  bsearch_deinit_root(&root);
}
END_TEST

START_TEST(test_binary_search_resize) {
  struct bsearch_root root;
  bsearch_init_root(&root, bsearch_key_uint32cmp, bsearch_key_nocopy, NULL,
                    NULL);

  for (uint32_t i = 0; i < 1000; i++) {
    void* val = CAST_u32_pvoid(i);
    bsearch_unsafe_append(&root, val, val);
  }

  ck_assert_int_eq(root.size, 1000);
  ck_assert_int_eq(root.capacity, 1024);
  bsearch_trim_capacity(&root);
  ck_assert_int_eq(root.capacity, 1000);

  ck_assert_int_eq(bsearch_get_val_u32(&root, 20, 0), 20);
  ck_assert_int_eq(bsearch_find_u32(&root, 20), 20);
  ck_assert_int_eq(bsearch_find_u32(&root, 2000), -1001);

  bsearch_deinit_root(&root);
}

START_TEST(test_binary_search_free_keys) {
  struct bsearch_root root;
  bsearch_init_root(&root, bsearch_key_strcmp, bsearch_key_strdup, free, NULL);

  bsearch_upsert(&root, "dog", "woof");
  bsearch_upsert(&root, "cow", "moo");
  bsearch_upsert(&root, "frog", "ribbit");

  bsearch_remove(&root, 0);
  bsearch_replace(&root, 0, "cat", "meow");

  ck_assert_int_eq(root.size, 2);
  ck_assert_pstr_eq(bsearch_get_val(&root, "cat", NULL), "meow");
  ck_assert_pstr_eq(bsearch_get_val(&root, "frog", NULL), "ribbit");

  ck_assert_pstr_eq(bsearch_get_val(&root, "dog", NULL), NULL);
  ck_assert_pstr_eq(bsearch_get_val(&root, "cow", NULL), NULL);

  bsearch_deinit_root(&root);
}

START_TEST(test_binary_search_root_ptr) {
  struct bsearch_root* root =
      bsearch_create_root(bsearch_key_strcmp, bsearch_key_strdup, free, NULL);
  ck_assert_ptr_ne(root, NULL);

  bsearch_upsert(root, "dog", "woof");
  bsearch_upsert(root, "cow", "moo");
  bsearch_upsert(root, "frog", "ribbit");

  ck_assert_int_eq(root->size, 3);
  ck_assert_pstr_eq(bsearch_get_val(root, "dog", NULL), "woof");
  ck_assert_pstr_eq(bsearch_get_val(root, "cow", NULL), "moo");
  ck_assert_pstr_eq(bsearch_get_val(root, "frog", NULL), "ribbit");

  bsearch_destroy_root(root);
}

START_TEST(test_binary_search_free_values) {
  struct bsearch_root root;
  bsearch_init_root(&root, bsearch_key_strcmp, bsearch_key_nocopy, NULL, free);

  bsearch_upsert(&root, "dog", strdup("woof"));
  bsearch_upsert(&root, "cow", strdup("moo"));
  bsearch_upsert(&root, "frog", strdup("ribbit"));

  bsearch_replace(&root, 2, "giraffe", strdup("neigh"));
  bsearch_remove(&root, 0);
  bsearch_upsert(&root, "dog", strdup("bark"));

  ck_assert_int_eq(root.size, 2);
  ck_assert_pstr_eq(bsearch_get_val(&root, "dog", NULL), "bark");
  ck_assert_pstr_eq(bsearch_get_val(&root, "giraffe", NULL), "neigh");
  ck_assert_pstr_eq(bsearch_get_val(&root, "cow", NULL), NULL);
  ck_assert_pstr_eq(bsearch_get_val(&root, "frog", NULL), NULL);

  bsearch_deinit_root(&root);
}

Suite* bsearch_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create("Binary Search");

  tc_core = tcase_create("Core");
  tcase_add_test(tc_core, test_binary_search_upsert_u32);
  tcase_add_test(tc_core, test_binary_search_delete_u32);
  tcase_add_test(tc_core, test_binary_search_find_u32);
  tcase_add_test(tc_core, test_binary_search_string_keys);
  tcase_add_test(tc_core, test_binary_search_resize);
  tcase_add_test(tc_core, test_binary_search_free_keys);
  tcase_add_test(tc_core, test_binary_search_root_ptr);
  tcase_add_test(tc_core, test_binary_search_free_values);
  suite_add_tcase(s, tc_core);

  return s;
}
