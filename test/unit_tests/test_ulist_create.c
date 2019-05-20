#include "unity.h"

#include "ulist_api.h"

#define NODE_SIZE (9u)

static ulist_t list;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_create_list_null(void)
{
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_create(NULL, sizeof(int),
        NODE_SIZE));
}

void test_create_size_zero(void)
{
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_create(&list, 0, NODE_SIZE));
}

void test_create_items_zero(void)
{
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_create(&list, sizeof(int), 0));
}

void test_create_success(void)
{
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&list, sizeof(int), NODE_SIZE));
    TEST_ASSERT_NOT_EQUAL(NULL, list.head);
    TEST_ASSERT_NOT_EQUAL(NULL, list.tail);
    TEST_ASSERT_EQUAL(0, list.num_items);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_create_list_null);
    RUN_TEST(test_create_size_zero);
    RUN_TEST(test_create_items_zero);
    RUN_TEST(test_create_success);
    return UNITY_END();
}
