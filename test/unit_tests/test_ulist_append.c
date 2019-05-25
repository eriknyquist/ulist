#include "unity.h"

#include "ulist_api.h"

// Number of items per node
#define NODE_SIZE (32u)

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

void test_append_num_items(void)
{
    int val = 5;

    TEST_ASSERT_EQUAL(0u, list.num_items);

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &val));
    TEST_ASSERT_EQUAL(1u, list.num_items);

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &val));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &val));
    TEST_ASSERT_EQUAL(3u, list.num_items);

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &val));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &val));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &val));
    TEST_ASSERT_EQUAL(6u, list.num_items);
}

void test_append_ascending_loop(void)
{
    int num_test_values = 1000u;
    for (int i = 0; i < num_test_values; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
        TEST_ASSERT_EQUAL(i + 1, list.num_items);
    }

    for (int i = 0; i < num_test_values; i++)
    {
        int val;
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_item(&list, i, &val));
        TEST_ASSERT_EQUAL(val, i);
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_append_list_null);
    RUN_TEST(test_append_data_null);
    RUN_TEST(test_append_num_items);
    RUN_TEST(test_append_ascending_loop);
    return UNITY_END();
}
