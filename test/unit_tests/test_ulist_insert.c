#include "unity.h"

#include "ulist_api.h"

#define NODE_SIZE (3u)

static ulist_t list;

void setUp(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&list, sizeof(int), NODE_SIZE));
}

void tearDown(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_destroy(&list));
}

void test_insert_list_null(void)
{
    int val = 7;
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_insert_item(NULL, 0u, &val));
}

void test_insert_data_null(void)
{
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_insert_item(&list, 0u, NULL));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_insert_list_null);
    RUN_TEST(test_insert_data_null);
    return UNITY_END();
}
