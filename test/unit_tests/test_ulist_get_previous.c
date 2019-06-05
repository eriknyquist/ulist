#include "unity.h"

#include "ulist_api.h"

#define NODE_SIZE (4u)

static ulist_t list;

void setUp(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&list, sizeof(int), NODE_SIZE));
}

void tearDown(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_destroy(&list));
}

void test_get_previous_list_null(void)
{
    int write_data = 7;
    void *val;

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_get_previous_item(NULL, &val));
}

void test_get_previous_data_null(void)
{
    int write_data = 7;

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_get_previous_item(&list, NULL));
}

void test_get_previous_empty(void)
{
    void *data;

    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_previous_item(&list, &data));
    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_previous_item(&list, &data));
}

void test_get_previous_everything(void)
{
    int num_items = 1000;

    TEST_ASSERT_EQUAL(list.num_items, 0u);

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
    }

    TEST_ASSERT_EQUAL(list.num_items, num_items);

    void *read_val;

    // Should be able to iterate until ULIST_END, and then it should reset so
    // we should be able to iterate again until ULIST_END. First time...
    for (int i = 0; i < num_items; i++)
    {

        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_previous_item(&list, &read_val));
        TEST_ASSERT_EQUAL(*(int *)read_val, (num_items - i) - 1);
    }

    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_previous_item(&list, &read_val));
    TEST_ASSERT_EQUAL(list.num_items, num_items);

    // Second time...
    for (int i = 0; i < num_items; i++)
    {

        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_previous_item(&list, &read_val));
        TEST_ASSERT_EQUAL(*(int *)read_val, (num_items - i) - 1);
    }

    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_previous_item(&list, &read_val));
    TEST_ASSERT_EQUAL(list.num_items, num_items);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_get_previous_list_null);
    RUN_TEST(test_get_previous_data_null);
    RUN_TEST(test_get_previous_empty);
    RUN_TEST(test_get_previous_everything);
    return UNITY_END();
}
