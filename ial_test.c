/**
 * Copyright (c) 2015, Team Unknown,
 *                     Ján Mochňak,    <xmochn00@vutbr.cz>
 *                     Tibor Dudlák,   <xdudla00@vutbr.cz>
 *                     Dávid Prexta,   <xprext00@vutbr.cz>
 *                     Martin Krajňák, <xkrajn02@vutbr.cz>
 *                     Patrik Segedy,  <xseged00@vutbr.cz>
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <cgreen/cgreen.h>
#include "ial.h"

Describe(Ial)
BeforeEach(Ial) {}
AfterEach(Ial) {}

Ensure(Ial, length_of_string_is_calculated_correctly) {
    assert_that(length("123"), is_equal_to(3));
    assert_that(length("12\n3"), is_equal_to(4));
    assert_that(length(""), is_equal_to(0));
}

Ensure(Ial, should_correctly_concate_two_strings) {
    char *str;

    str = concat("foo", "bazbar");
    assert_string_equal(str, "foobazbar");

    free(str);
}

Ensure(Ial, should_correctly_take_part_of_string) {
    char *str;

    str = substr("__foo__", 2, 3);
    assert_string_equal(str, "foo");
    free(str);

    str = substr("__foo__", 7, 3);
    assert_string_equal(str, "");
    free(str);

    str = substr("__foo__", 3, 75);
    assert_string_equal(str, "oo__");
    free(str);
}

TestSuite *ial_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Ial, length_of_string_is_calculated_correctly);
    add_test_with_context(suite, Ial, should_correctly_concate_two_strings);
    add_test_with_context(suite, Ial, should_correctly_take_part_of_string);
    return suite;
}
