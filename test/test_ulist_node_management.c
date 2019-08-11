#include "unity.h"

#include "ulist_api.h"

// Number of items per node
#define NODE_SIZE (4u)
#define HALF_FULL (2u)

static ulist_t list;

void setUp(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&list, sizeof(int), NODE_SIZE));
}

void tearDown(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_destroy(&list));
}

void _verify_node_item_ratio(ulist_t *list)
{
    ulist_node_t *node = list->head;
    size_t item_count = 0;
    size_t node_count = 0;

    while (NULL != node)
    {
        if (node != list->tail)
        {
            TEST_ASSERT_TRUE(node->used >= HALF_FULL);
        }

        item_count += node->used;
        node_count += 1u;
        node = node->next;
    }

    TEST_ASSERT_EQUAL(list->num_items, item_count);
    TEST_ASSERT_EQUAL(list->nodes, node_count);
}

void test_node_grow_rate_append(void)
{
    size_t num_items = NODE_SIZE * 10;

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
        TEST_ASSERT_EQUAL(list.num_items, i + 1);
        _verify_node_item_ratio(&list);
    }
}

void test_node_grow_rate_prepend(void)
{
    size_t num_items = NODE_SIZE * 10;

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, 0u, &i));
        TEST_ASSERT_EQUAL(list.num_items, i + 1);
        _verify_node_item_ratio(&list);
    }
}

void test_node_grow_rate_append_insert(void)
{
    size_t num_items = NODE_SIZE * 10;

    for (int i = 0; i < (num_items / 2); i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
        TEST_ASSERT_EQUAL(list.num_items, i + 1);
        _verify_node_item_ratio(&list);
    }

    for (int i = 0; i < (num_items / 2); i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_insert_item(&list, 5u, &i));
        TEST_ASSERT_EQUAL(list.num_items, (num_items / 2) + i + 1);
        _verify_node_item_ratio(&list);
    }
}

void test_node_shrink_rate_tail(void)
{
    size_t num_items = NODE_SIZE * 10;

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
    }

    TEST_ASSERT_EQUAL(num_items / NODE_SIZE, list.nodes);

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_pop_item(&list, list.num_items - 1, NULL));
        TEST_ASSERT_EQUAL(list.num_items, num_items -  (i + 1));
        _verify_node_item_ratio(&list);
    }
}

void test_node_shrink_rate_head(void)
{
    size_t num_items = NODE_SIZE * 10;

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
    }

    TEST_ASSERT_EQUAL(num_items / NODE_SIZE, list.nodes);

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_pop_item(&list, 0u, NULL));
        TEST_ASSERT_EQUAL(list.num_items, num_items -  (i + 1));
        _verify_node_item_ratio(&list);
    }
}

void test_node_shrink_rate_head_middle(void)
{
    size_t num_items = NODE_SIZE * 10;

    for (int i = 0; i < num_items; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
    }

    TEST_ASSERT_EQUAL(num_items / NODE_SIZE, list.nodes);

    for (int i = 0; i < (num_items - 3); i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_pop_item(&list, 3u, NULL));
        TEST_ASSERT_EQUAL(list.num_items, num_items -  (i + 1));
        _verify_node_item_ratio(&list);
    }

    for (int i = 0; i < 3; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_pop_item(&list, 0u, NULL));
        _verify_node_item_ratio(&list);
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_node_grow_rate_append);
    RUN_TEST(test_node_grow_rate_prepend);
    RUN_TEST(test_node_grow_rate_append_insert);
    RUN_TEST(test_node_shrink_rate_tail);
    RUN_TEST(test_node_shrink_rate_head);
    RUN_TEST(test_node_shrink_rate_head_middle);
    return UNITY_END();
}
