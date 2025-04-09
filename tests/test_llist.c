#include "../include/llist.h"
#include "test_utils.h"
#include "test_log.h"
#include <stdlib.h>
#include <stdio.h>

void test_create_destroy() {
    TEST_START;
	TEST_LOG("Creating list with MT_SUPPORT_FALSE");
    llist_t *list = llist_create(MT_SUPPORT_FALSE);
    ASSERT_NOT_NULL(list);
    ASSERT_EQ(0, llist_size(list));
    ASSERT_EQ(1, llist_is_empty(list));
    ASSERT_EQ(0, llist_destroy(list, 0, NULL));
    TEST_PASS;
}

void test_add_nodes() {
    TEST_START;
	TEST_LOG("Creating list with MT_SUPPORT_FALSE");
    llist_t *list = llist_create(MT_SUPPORT_FALSE);
    int data1 = 42, data2 = 100;
    
    ASSERT_EQ(0, llist_add_node(list, &data1, ADD_NODE_FRONT));
    ASSERT_EQ(1, llist_size(list));
    ASSERT_EQ(0, llist_add_node(list, &data2, ADD_NODE_REAR));
    ASSERT_EQ(2, llist_size(list));
    
    llist_destroy(list, 0, NULL);
    TEST_PASS;
}

int main() {
    test_create_destroy();
    test_add_nodes();
    printf("All tests passed!\n");
    return 0;
}
