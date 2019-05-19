#include "unity.h"

#include "ulist_api.h"

#define NODE_SIZE (3u)

static ulist_t list;

static int _int_in_list(int val, int *list, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (val == list[i])
        {
            return 1;
        }
    }

    return 0;
}

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

void test_insert_empty_index_range_1(void)
{
    int val = 7;
    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_insert_item(&list, 1u, &val));
}

void test_insert_empty_index_range_2(void)
{
    int val = 7;
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, 0u, &val));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, 1u, &val));
    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_insert_item(&list, 3u, &val));
}

void test_insert_end(void)
{
    int val1 = 7777;
    int val2 = 8888;
    int val3 = 9999;

    int *out_val1, *out_val2, *out_val3;

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, 0u, &val1));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, 1u, &val2));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, 2u, &val3));

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_item(&list, 0u, (void **)&out_val1));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_item(&list, 1u, (void **)&out_val2));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_item(&list, 2u, (void **)&out_val3));

    TEST_ASSERT_EQUAL(*out_val1, val1);
    TEST_ASSERT_EQUAL(*out_val2, val2);
    TEST_ASSERT_EQUAL(*out_val3, val3);
}

void test_insert_num_items(void)
{
    int num_test_values = 1000;
    int *out_val;

    for (int i = 0; i < num_test_values; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, list.num_items, &i));
    }

    for (int i = 0; i < num_test_values; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_item(&list, i, (void **)&out_val));
        TEST_ASSERT_EQUAL(*out_val, i);
    }
}

void test_insert_random_inserts(void)
{
    int insert_indices[10] = { 4, 18, 205, 250, 348, 444, 598, 770, 899, 989 };
    int num_test_values = 1000;
    int *out_val;

    int num_indices = sizeof(insert_indices) / sizeof(int);

    // build list with simple ascending pattern
    for (int i = 0; i < num_test_values; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, list.num_items, &i));
    }

    // insert other known values at locations in insert_indices
    for (int i = 0; i < num_indices; i++)
    {
        int insertval = i + 100;
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, insert_indices[i], &insertval));
    }

    int indices_found = 0;

    // verify list now reports expected number of items
    TEST_ASSERT_EQUAL(list.num_items, num_test_values + num_indices);

    // verify all original list items exist, and inserted items are in their
    // expected locations
    for (int i = 0; i < list.num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_item(&list, i, (void **)&out_val));

        if (_int_in_list(i, insert_indices, num_indices))
        {
            TEST_ASSERT_EQUAL(*out_val, indices_found + 100);
            indices_found += 1;
        }
        else
        {
            TEST_ASSERT_EQUAL(*out_val, i - indices_found);
        }
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_insert_list_null);
    RUN_TEST(test_insert_data_null);
    RUN_TEST(test_insert_empty_index_range_1);
    RUN_TEST(test_insert_empty_index_range_2);
    RUN_TEST(test_insert_end);
    RUN_TEST(test_insert_num_items);
    RUN_TEST(test_insert_random_inserts);
    return UNITY_END();
}
