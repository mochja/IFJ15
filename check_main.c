#include <stdlib.h>
#include <check.h>
#include "ial.h"

START_TEST(test_ial_length)
{
    ck_assert_int_eq(length("asdf"), 4);
    ck_assert_int_eq(length("x\nz"), 3);
}
END_TEST

Suite* ial_suite(void)
{
    Suite* s;
    TCase* tc_internals;

    s = suite_create("IAL");

    tc_internals = tcase_create("ial_internals");

    tcase_add_test(tc_internals, test_ial_length);
    suite_add_tcase(s, tc_internals);

    return s;
}

int main(void)
{
    int number_failed;
    SRunner* sr;

    sr = srunner_create(ial_suite());
    // srunner_add_suite(sr)

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
