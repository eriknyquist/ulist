/*
 * ulist.c
 */
#include <string.h>
#include <stdlib.h>
#include "ulist_api.h"

#define NODE_ALLOC_SIZE(list) (sizeof(ulist_node_t) +      \
                                   list->item_size_bytes + \
                                   list->items_per_node)

// Struct to hold parameters required to access a single data item in list
typedef struct {
    ulist_node_t *node;
    size_t local_index;
} access_params_t;

// Allocate a new node and return a pointer to it
static ulist_node_t *_new_node(ulist_t *list)
{
    ulist_node_t *node;

    if ((node = malloc(NODE_ALLOC_SIZE(list))) == NULL)
    {
        return NULL;
    }

    return node;
}

// Find a specific data item by index and fill out an access_params_t instance
static access_params_t *_find_item_by_index(ulist_t *list, unsigned long long index,
    access_params_t *access_params)
{
    ulist_node_t *node = NULL;
    size_t local_index;

    if (NULL == access_params)
    {
        return NULL;
    }

    // First item
    if (0u == index)
    {
        node = list->head;
        local_index = 0u;
    }
    // Last item
    else if ((list->num_items - 1u) == index)
    {
        node = list->tail;
        local_index = list->tail->used - 1u;
    }
    // Somewhere in between
    else
    {
        node = list->head;
        unsigned long long total_items = 0u;
        size_t local_index;

        while (NULL != node)
        {
            total_items += node->used;
            if (total_items >= (index + 1))
            {
                break;
            }

            node = node->next;
        }

        local_index = total_items - (index + 1);
    }

    if (NULL == node)
    {
        return NULL;
    }

    access_params->node = (ulist_node_t *)node;
    access_params->local_index = local_index;
    return access_params;
}

ulist_status_e ulist_node_size_bytes(ulist_t *list, size_t *size_bytes)
{
    if ((NULL == list) || (NULL == size_bytes))
    {
        return ULIST_INVALID_PARAM;
    }

    *size_bytes = NODE_ALLOC_SIZE(list);

    return ULIST_OK;
}

ulist_status_e ulist_create(ulist_t *list, size_t item_size_bytes,
    size_t items_per_node)
{
    ulist_status_e err;
    if (head || tail)
    {
        return ULIST_ALREADY_INIT;
    }

    if ((NULL == list) || (0u == item_size_bytes) || (0u == items_per_node))
    {
        return ULIST_INVALID_PARAM;
    }

    memset(list, 0, sizeof(list_t));
    list->item_size_bytes = item_size_bytes;
    list->items_per_node = items_per_node;

    if ((list->head = _new_node(list)) == NULL)
    {
        return ULIST_ERROR_MEM;
    }

    list->tail = list->head;
    return ULIST_OK;
}

ulist_status_e ulist_destroy(ulist_t *list)
{
    ulist_node_t *node, *old;
    if (NULL == list)
    {
        return ULIST_INVALID_PARAM;
    }

    if (list->head && list->tail)
    {
        node = list->head;
        while (NULL != node)
        {
            old = node;
            node = node->next;
            free(old);
        }
    }

    list->head = NULL;
    list->tail = NULL;
    return ULIST_OK;
}

ulist_status_e ulist_append_item(ulist_t *list, void *item)
{
    if ((NULL == list) || (NULL == list->tail) || (NULL == item))
    {
        return ULIST_INVALID_PARAM;
    }

    return ULIST_OK;
}

ulist_status_e ulist_insert_item(ulist_t *list, unsigned long long index, void *item)
{
    if ((NULL == list) || (NULL == list->tail) || (NULL == item))
    {
        return ULIST_INVALID_PARAM;
    }

    if (index > list->num_items)
    {
        return ULIST_INDEX_OUT_OF_RANGE;
    }

    if (index == list->num_items)
    {

    }

    access_params_t params;
    if (_find_item_by_index(list, index, &params) == NULL)
    {
        return ULIST_ERROR_INTERNAL;
    }

    ulist_node_t *new_node;
    if ((new_node = _new_node(list)) == NULL)
    {
        return ULIST_ERROR_MEM;
    }

    return ULIST_OK;
}

ulist_status_e ulist_get_item(ulist_t *list, unsigned long long index)
{
    if ((NULL == list) || (NULL == list->tail))
    {
        return ULIST_INVALID_PARAM;
    }

    if (index >= list->num_items)
    {
        return ULIST_INDEX_OUT_OF_RANGE;
    }


}
