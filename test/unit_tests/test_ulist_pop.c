#include "unity.h"

#include "ulist_api.h"

#define NODE_SIZE (4u)

static ulist_t list;

void _dump_list(ulist_t *list)
{
    int num = 1;
    ulist_node_t *node = list->head;

    while (NULL != node)
    {
        printf("Node %d\n", num);
        int *data = (int *)node->data;

        for (int i = 0; i < node->used; i++)
        {
            printf("  %d (%p)\n", data[i], data + i);
        }
        printf("\n");

        num += 1;
        node = node->next;
    }
}

void setUp(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&list, sizeof(int), NODE_SIZE));
}

void tearDown(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_destroy(&list));
}

void test_pop_list_null(void)
{
    int val;
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_pop_item(NULL, 0u, &val));
}

void test_pop_data_null(void)
{
    int write_data = 7;

    TEST_ASSERT_EQUAL(list.num_items, 0u);
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, 0u, &write_data));
    TEST_ASSERT_EQUAL(list.num_items, 1u);
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_pop_item(&list, 0u, NULL));
    TEST_ASSERT_EQUAL(list.num_items, 0u);
}

void test_pop_empty(void)
{
    int data;

    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_pop_item(&list, 0u, &data));
    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_pop_item(&list, 1u, &data));
}

void test_pop_index_out_of_range(void)
{
    int data = 1265;

    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_pop_item(&list, 0u, &data));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, 0u, &data));
    TEST_ASSERT_EQUAL(ULIST_INDEX_OUT_OF_RANGE, ulist_pop_item(&list, 1u, &data));
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_pop_item(&list, 0u, NULL));
}

void test_pop_everything_from_head(void)
{
    int num_items = 1000;

    TEST_ASSERT_EQUAL(list.num_items, 0u);

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, list.num_items, &i));
    }

    TEST_ASSERT_EQUAL(list.num_items, num_items);

    for (int i = 0; i < num_items; i++)
    {
        int read_val;

        TEST_ASSERT_EQUAL(ULIST_OK, ulist_pop_item(&list, 0u, &read_val));
        TEST_ASSERT_EQUAL(read_val, i);

        for (int j = 0; j < list.num_items; j++)
        {
            int check_val;
            TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_item(&list, j, &check_val));
            TEST_ASSERT_EQUAL(i + j + 1, check_val);
        }
    }

    TEST_ASSERT_EQUAL(list.num_items, 0u);
}

void test_pop_everything_from_tail(void)
{
    int num_items = 1000;

    TEST_ASSERT_EQUAL(list.num_items, 0u);

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, list.num_items, &i));
    }

    TEST_ASSERT_EQUAL(list.num_items, num_items);

    for (int i = 0; i < num_items; i++)
    {
        int read_val;

        TEST_ASSERT_EQUAL(ULIST_OK, ulist_pop_item(&list, list.num_items - 1, &read_val));
        TEST_ASSERT_EQUAL(read_val, (num_items - 1) - i);

        for (int j = 0; j < list.num_items; j++)
        {
            int check_val;
            TEST_ASSERT_EQUAL(ULIST_OK, ulist_get_item(&list, j, &check_val));
            TEST_ASSERT_EQUAL(j, check_val);
        }
    }

    TEST_ASSERT_EQUAL(list.num_items, 0u);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_pop_list_null);
    RUN_TEST(test_pop_data_null);
    RUN_TEST(test_pop_empty);
    RUN_TEST(test_pop_index_out_of_range);
    RUN_TEST(test_pop_everything_from_head);
    RUN_TEST(test_pop_everything_from_tail);
    return UNITY_END();
}
