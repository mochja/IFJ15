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
 * license.txt file in the root directory of this source tree.
 */

#include <cgreen/cgreen.h>
#include "ial.h"

Describe(Hash)
BeforeEach(Hash) {}
AfterEach(Hash) {}

Ensure(Hash, has_correct_size_after_init) {
    tTable *table;
    table = initHashTable(10);
    assert_that(table->size, is_equal_to(10));
    freeHashTable(table);
}

Ensure(Hash, can_hold_an_item) {
    tTable *table;

    table = initHashTable(10);
    assert_that(table->size, is_equal_to(10));

    hTabItem *item;
    item = createNewItem();
    assert_that(item, is_not_equal_to(NULL));

    item->name = malloc(sizeof(char) * (10 + 1));
    strcpy(item->name, "test");
    item->dataType = 18;
    item->sVal = malloc(sizeof(char) * (10 + 1));
    strcpy(item->sVal, "abcdabcd12");

    insertHashTable(table, item);

    hTabItem *ret = searchItem(table, "test");
    assert_that(ret, is_not_equal_to(NULL));
    assert_that(ret, is_equal_to(item));
    assert_that(ret->dataType, is_equal_to(18));
    assert_that(ret->dataType, is_equal_to(18));
    assert_string_equal("abcdabcd12", ret->sVal);
    free(item->sVal);

    freeHashTable(table);
}

TestSuite *hash_suite() {
    TestSuite *suite = create_test_suite();
    add_test_with_context(suite, Hash, has_correct_size_after_init);
    add_test_with_context(suite, Hash, can_hold_an_item);
    return suite;
}
