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

void test_get_next_list_null(void)
{
    int write_data = 7;
    int val = 0;

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_get_next_item(NULL, &val));
    TEST_ASSERT_EQUAL(val, 0);
}

void test_get_next_data_null(void)
{
    int write_data = 7;

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_get_next_item(&list, NULL));
}

void test_get_next_empty(void)
{
    int data;

    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_next_item(&list, &data));
    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_next_item(&list, &data));
}

void test_get_next_everything(void)
{
    int num_items = 1000;

    TEST_ASSERT_EQUAL(list.num_items, 0u);

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
    }

    TEST_ASSERT_EQUAL(list.num_items, num_items);

    int read_val;

    // Should be able to iterate until ULIST_END, and then it should reset so
    // we should be able to iterate again until ULIST_END. First time...
    for (int i = 0; i < num_items; i++)
    {

        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_next_item(&list, &read_val));
        TEST_ASSERT_EQUAL(read_val, i);
    }

    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_next_item(&list, &read_val));
    TEST_ASSERT_EQUAL(list.num_items, num_items);

    // Second time...
    for (int i = 0; i < num_items; i++)
    {

        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_next_item(&list, &read_val));
        TEST_ASSERT_EQUAL(read_val, i);
    }

    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_next_item(&list, &read_val));
    TEST_ASSERT_EQUAL(list.num_items, num_items);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_get_next_list_null);
    RUN_TEST(test_get_next_data_null);
    RUN_TEST(test_get_next_empty);
    RUN_TEST(test_get_next_everything);
    return UNITY_END();
}
