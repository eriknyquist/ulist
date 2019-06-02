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

void test_set_iteration_index_list_null(void)
{
    int write_data = 7;
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_set_iteration_start_index(NULL, 0u));
}

void test_set_iteration_index_empty(void)
{
    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_set_iteration_start_index(&list, 0u));
}

void test_set_iteration_index_out_of_range(void)
{
    int write_data = 7;

    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_set_iteration_start_index(&list, 0u));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_set_iteration_start_index(&list, 1u));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_set_iteration_start_index(&list, 2u));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_set_iteration_start_index(&list, 1u));
}

void test_set_iteration_index_get_next(void)
{
    int num_items = 1000;
    unsigned long long start_index = 643;

    TEST_ASSERT_EQUAL(list.num_items, 0u);

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
    }

    TEST_ASSERT_EQUAL(list.num_items, num_items);

    int read_val;
    int num_iter_items = num_items - start_index;

    // Should be able to iterate until ULIST_END, and then it should reset so
    // we should be able to iterate again until ULIST_END. First time...

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_set_iteration_start_index(&list, start_index));

    for (int i = 0; i < num_iter_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_next_item(&list, &read_val));
        TEST_ASSERT_EQUAL(read_val, i + start_index);
    }

    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_next_item(&list, &read_val));
    TEST_ASSERT_EQUAL(list.num_items, num_items);

    // Second time...
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_set_iteration_start_index(&list, start_index));

    for (int i = 0; i < num_iter_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_next_item(&list, &read_val));
        TEST_ASSERT_EQUAL(read_val, i + start_index);
    }

    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_next_item(&list, &read_val));
    TEST_ASSERT_EQUAL(list.num_items, num_items);
}

void test_set_iteration_index_get_previous(void)
{
    int num_items = 1000;
    unsigned long long start_index = 643;

    TEST_ASSERT_EQUAL(list.num_items, 0u);

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
    }

    TEST_ASSERT_EQUAL(list.num_items, num_items);

    int read_val;
    int num_iter_items = start_index + 1;

    // Should be able to iterate until ULIST_END, and then it should reset so
    // we should be able to iterate again until ULIST_END. First time...

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_set_iteration_start_index(&list, start_index));

    for (int i = 0; i < num_iter_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_previous_item(&list, &read_val));
        TEST_ASSERT_EQUAL(read_val, start_index - i);
    }

    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_previous_item(&list, &read_val));
    TEST_ASSERT_EQUAL(list.num_items, num_items);

    // Second time...
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_set_iteration_start_index(&list, start_index));

    for (int i = 0; i < num_iter_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_previous_item(&list, &read_val));
        TEST_ASSERT_EQUAL(read_val, start_index - i);
    }

    TEST_ASSERT_EQUAL(ULIST_END, ulist_get_previous_item(&list, &read_val));
    TEST_ASSERT_EQUAL(list.num_items, num_items);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_set_iteration_index_list_null);
    RUN_TEST(test_set_iteration_index_empty);
    RUN_TEST(test_set_iteration_index_out_of_range);
    RUN_TEST(test_set_iteration_index_get_next);
    RUN_TEST(test_set_iteration_index_get_previous);
    return UNITY_END();
}
