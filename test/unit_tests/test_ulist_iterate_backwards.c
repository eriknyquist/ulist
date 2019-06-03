#include "unity.h"

#include "ulist_api.h"

#define NODE_SIZE (5u)

#define NUM_TEST_ITEMS (1000)
#define EARLY_STOP_INDEX (211u)

static ulist_t list;
static int _unexpected_value_found;
static int _handler_invocations;

void setUp(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_create(&list, sizeof(int), NODE_SIZE));
   _unexpected_value_found = 0;
   _handler_invocations = 0;
}

void tearDown(void)
{
   TEST_ASSERT_EQUAL(ULIST_OK, ulist_destroy(&list));
}

static int _dummy_handler(unsigned long long index, void *item)
{
    _handler_invocations += 1;
    return 0;
}

static int _value_check_handler(unsigned long long index, void *item)
{
    int *val = (int *)item;

    _handler_invocations += 1;

    if (*val != index)
    {
        _unexpected_value_found = 1;
        return 1;
    }

    return 0;
}

static int _early_stop_handler(unsigned long long index, void *item)
{
    int *val = (int *)item;

    _handler_invocations += 1;

    if (*val != index)
    {
        _unexpected_value_found = 1;
        return 1;
    }

    if (EARLY_STOP_INDEX == index)
    {
        return 1;
    }

    return 0;
}

void test_iterate_backwards_list_null(void)
{
    int write_data = 7;

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_iterate_backwards(NULL, _dummy_handler));
}

void test_iterate_backwards_handler_null(void)
{
    int write_data = 7;

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &write_data));
    TEST_ASSERT_EQUAL(ULIST_INVALID_PARAM, ulist_iterate_backwards(&list, NULL));
}

void test_iterate_backwards_success_empty(void)
{
    TEST_ASSERT_EQUAL(ULIST_OK, ulist_iterate_backwards(&list, _dummy_handler));
}

void test_iterate_backwards_check_items(void)
{
    for (int i = 0; i < NUM_TEST_ITEMS; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
    }

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_iterate_backwards(&list, _value_check_handler));
    TEST_ASSERT_EQUAL(0, _unexpected_value_found);
    TEST_ASSERT_EQUAL(NUM_TEST_ITEMS, _handler_invocations);
}

void test_iterate_backwards_check_items_stop_early(void)
{
    for (int i = 0; i < NUM_TEST_ITEMS; i++)
    {
        TEST_ASSERT_EQUAL(ULIST_OK, ulist_append_item(&list, &i));
    }

    TEST_ASSERT_EQUAL(ULIST_OK, ulist_iterate_backwards(&list, _early_stop_handler));
    TEST_ASSERT_EQUAL(0, _unexpected_value_found);
    TEST_ASSERT_EQUAL(NUM_TEST_ITEMS - EARLY_STOP_INDEX, _handler_invocations);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_iterate_backwards_list_null);
    RUN_TEST(test_iterate_backwards_handler_null);
    RUN_TEST(test_iterate_backwards_success_empty);
    RUN_TEST(test_iterate_backwards_check_items);
    RUN_TEST(test_iterate_backwards_check_items_stop_early);
    return UNITY_END();
}
