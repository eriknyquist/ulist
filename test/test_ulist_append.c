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

void _do_grow_rate_test(size_t node_size, size_t total_items)
{
    ulist_t temp_list;
    size_t expected_num_nodes = 1u;
    unsigned just_crossed_node_boundary = 0u;

   TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&temp_list, sizeof(int), node_size));

    for (int i = 0; i < total_items; i++)
    {
        /* If we just created a new node, verify the reported node count has
         * increased by 1 */
        if (just_crossed_node_boundary)
        {
            just_crossed_node_boundary = 0u;
            expected_num_nodes += 1u;
        }
        /* If we're just about to exceed the number of items that can fit in a
         * node, verify the reported node count is still the same */
        else if ((temp_list.num_items > 0u) && !(temp_list.num_items % node_size))
        {
            just_crossed_node_boundary = 1u;
        }

        TEST_ASSERT_EQUAL(expected_num_nodes, temp_list.nodes);
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&temp_list, &i));
    }

   TEST_ASSERT_EQUAL(ULIST_OK, ulist_destroy(&temp_list));
}

void test_append_nodes_grow_rate_2(void)
{
    _do_grow_rate_test(2, 1000);
}

void test_append_nodes_grow_rate_3(void)
{
    _do_grow_rate_test(3, 1000);
}

void test_append_nodes_grow_rate_7(void)
{
    _do_grow_rate_test(7, 1000);
}

void test_append_nodes_grow_rate_32(void)
{
    _do_grow_rate_test(32, 1000);
}

void test_append_nodes_grow_rate_45(void)
{
    _do_grow_rate_test(45, 1000);
}

void test_append_nodes_grow_rate_256(void)
{
    _do_grow_rate_test(256, 1000);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_append_list_null);
    RUN_TEST(test_append_data_null);
    RUN_TEST(test_append_num_items);
    RUN_TEST(test_append_ascending_loop);
    RUN_TEST(test_append_nodes_grow_rate_2);
    RUN_TEST(test_append_nodes_grow_rate_3);
    RUN_TEST(test_append_nodes_grow_rate_7);
    RUN_TEST(test_append_nodes_grow_rate_32);
    RUN_TEST(test_append_nodes_grow_rate_45);
    RUN_TEST(test_append_nodes_grow_rate_256);
    return UNITY_END();
}
