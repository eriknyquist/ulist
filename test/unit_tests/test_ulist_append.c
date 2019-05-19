#include "unity.h"

#include "ulist_api.h"

// Number of items per node
#define NODE_SIZE (8u)

static ulist_t list;

void setUp(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&list, sizeof(int), NODE_SIZE));
}

void tearDown(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_destroy(&list));
}

void test_append_list_null(void)
{
    int val = 7;
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_append_item(NULL, &val));
}

void test_append_data_null(void)
{
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_append_item(&list, NULL));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_append_list_null);
    RUN_TEST(test_append_data_null);
    return UNITY_END();
}
