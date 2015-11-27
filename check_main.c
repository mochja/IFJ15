#include <stdlib.h>
#include <check.h>
#include "ial.h"
#include "hash.h"

START_TEST(test_ial_length)
{
    ck_assert_int_eq(length("asdf"), 4);
    ck_assert_int_eq(length("x\nz"), 3);
}
END_TEST

START_TEST(test_ial_substr)
{
    char* str;
    const char base[] = "foobar";

    str = substr(base, 0, 3);
    ck_assert_str_eq(str, "foo");
    free(str);

    str = substr(base, 3, 3);
    ck_assert_str_eq(str, "bar");
    free(str);

    str = substr(base, 6, 3);
    ck_assert_str_eq(str, "");
    free(str);

    str = substr(base, 3, 0);
    ck_assert_str_eq(str, "");
    free(str);

    str = substr(base, 5, 30);
    ck_assert_str_eq(str, "r");
    free(str);
}
END_TEST

START_TEST(test_ial_concat)
{
    char *str;

    str = concat("test", "_is_rhododendron");
    ck_assert_str_eq(str, "test_is_rhododendron");
    free(str);

    str = concat("test", "");
    ck_assert_str_eq(str, "test");
    free(str);

    str = concat("", "123");
    ck_assert_str_eq(str, "123");
    free(str);

    str = concat("", "");
    ck_assert_str_eq(str, "");
    free(str);
}
END_TEST

Suite* ial_suite(void)
{
    Suite* s;
    TCase* tc_internals;

    s = suite_create("IAL");

    tc_internals = tcase_create("ial_internals");

    tcase_add_test(tc_internals, test_ial_length);
    tcase_add_test(tc_internals, test_ial_substr);
    tcase_add_test(tc_internals, test_ial_concat);

    suite_add_tcase(s, tc_internals);

    return s;
}


// --- HASH
START_TEST(test_hash_init)
{
    tTable *table;

    table = initHashTable(10);

    ck_assert_int_eq(10, table->size);

    freeHashTable(table);
}
END_TEST

START_TEST(test_hash_add_item)
{
    tTable *table;

    table = initHashTable(10);
    ck_assert_int_eq(10, table->size);

    hTabItem *item;
    item = createNewItem();
    ck_assert_ptr_ne(item, NULL);

    item->name = "test";
    item->dataType = 18;
    item->sVal = malloc(sizeof(char) * (10 + 1));
    strcpy(item->sVal, "abcdabcd12");

    insertHashTable(table, item);

    hTabItem *ret = searchItem(table, "test");
    ck_assert_ptr_ne(NULL, ret);
    ck_assert_ptr_eq(item, ret);
    ck_assert_int_eq(18, ret->dataType);
    ck_assert_str_eq("abcdabcd12", ret->sVal);
    free(item->sVal);

    freeHashTable(table);
}
END_TEST


Suite* hash_suite(void)
{
    Suite* s;
    TCase* tc_internals;

    s = suite_create("HASH");

    tc_internals = tcase_create("hash_table");

    tcase_add_test(tc_internals, test_hash_init);
    tcase_add_test(tc_internals, test_hash_add_item);

    suite_add_tcase(s, tc_internals);

    return s;
}

int main(void)
{
    int number_failed;
    SRunner* sr;

    sr = srunner_create(ial_suite());
    srunner_add_suite(sr, hash_suite());

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
