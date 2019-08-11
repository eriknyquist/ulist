#include "unity.h"

#include "ulist_api.h"

#define NODE_SIZE (5u)

static ulist_t list;

void setUp(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&list, sizeof(int), NODE_SIZE));
}

void tearDown(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_destroy(&list));
}

void test_get_list_null(void)
{
    int write_data = 7;
    int val = 0;

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_get_item(NULL, 0u, &val));
    TEST_ASSERT_EQUAL(val, 0);
}

void test_get_data_null(void)
{
    int write_data = 7;

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_get_item(&list, 0u, NULL));
}

void test_get_empty(void)
{
    int data;

    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_get_item(&list, 0u, &data));
    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_get_item(&list, 1u, &data));
}

void test_get_index_out_of_range(void)
{
    int write_data = 78265;
    int val = 0;

    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_get_item(&list, 0u, &val));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_get_item(&list, 1u, &val));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_item(&list, 0u, &val));
    TEST_ASSERT_EQUAL(val, write_data);
}

void test_get_everything(void)
{
    int num_items = 1000;

    TEST_ASSERT_EQUAL(list.num_items, 0u);

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
    }

    TEST_ASSERT_EQUAL(list.num_items, num_items);

    for (int i = 0; i < num_items; i++)
    {
        int read_val;

        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_item(&list, i, &read_val));
        TEST_ASSERT_EQUAL(read_val, i);
    }

    TEST_ASSERT_EQUAL(list.num_items, num_items);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_get_list_null);
    RUN_TEST(test_get_data_null);
    RUN_TEST(test_get_empty);
    RUN_TEST(test_get_index_out_of_range);
    RUN_TEST(test_get_everything);
    return UNITY_END();
}
