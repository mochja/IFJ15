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

TestSuite *hash_suite();
TestSuite *ial_suite();

int main(int argc, char **argv) {
    TestSuite *suite = create_test_suite();
    TestReporter *reporter = create_text_reporter();

    int result;

    if (argc > 1) {
        if (!strcmp("Hash", argv[1])) {
            add_suite(suite, hash_suite());
        } else if (!strcmp("Ial", argv[1])) {
            add_suite(suite, ial_suite());
        }

        if (argc > 2) {
            result = run_single_test(suite, argv[2], reporter);
        } else {
            result = run_test_suite(suite, reporter);
        }
    } else {
        add_suite(suite, hash_suite());
        add_suite(suite, ial_suite());
        result = run_test_suite(suite, reporter);
    }

    destroy_test_suite(suite);
    destroy_reporter(reporter);

    return result;
}