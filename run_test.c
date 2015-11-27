#include <cgreen/cgreen.h>

TestSuite *hash_suite();

int main(int argc, char **argv) {
    TestSuite *suite = create_test_suite();
    TestReporter *reporter = create_text_reporter();

    int result;

    if (argc > 1) {
        if (!strcmp("Hash", argv[1])) {
            add_suite(suite, hash_suite());
        }

        if (argc > 2) {
            result = run_single_test(suite, argv[2], reporter);
        } else {
            result = run_test_suite(suite, reporter);
        }
    } else {
        add_suite(suite, hash_suite());
        result = run_test_suite(suite, reporter);
    }

    destroy_test_suite(suite);
    destroy_reporter(reporter);

    return result;
}