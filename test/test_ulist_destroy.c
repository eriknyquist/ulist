#include "unity.h"

#include "ulist_api.h"

#define MIN_ITEMS_PER_NODE (2u)
#define NODE_SIZE MIN_ITEMS_PER_NODE

static ulist_t list;

void setUp(void)
{
}

void tearDown(void)
{
}

void test_destroy_list_null(void)
{
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_destroy(NULL));
}

void test_destroy_success(void)
{
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&list, sizeof(int), NODE_SIZE));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_destroy(&list));
}

void test_destroy_list_already_destroyed(void)
{
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&list, sizeof(int), NODE_SIZE));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_destroy(&list));
    TEST_ASSERT_EQUAL(ULIST_ALREADY_DESTROYED, ulist_destroy(&list));
    TEST_ASSERT_EQUAL(ULIST_ALREADY_DESTROYED, ulist_destroy(&list));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_destroy_list_null);
    RUN_TEST(test_destroy_success);
    RUN_TEST(test_destroy_list_already_destroyed);
    return UNITY_END();
}
