/*
 * ulist.c
 */
#include <string.h>
#include "ulist_api.h"

#define NODE_ALLOC_SIZE(list) (sizeof(ulist_node_t) +       \
                                   (list->item_size_bytes * \
                                   list->items_per_node))

#define NODE_DATA(list, node, i) (node->data + (list->item_size_bytes * (i)))

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

    memset(node, 0, sizeof(ulist_node_t));
    return node;
}

// Add item to node, creating a new node if required
static ulist_status_e _add_item_to_node(ulist_t *list, ulist_node_t *node,
        size_t index, void *item)
{
    ulist_node_t *new = node;

    // Current node is full, create a new one
    if (node->used == list->items_per_node)
    {
        if ((new = _new_node(list)) == NULL)
        {
            return ULIST_ERROR_MEM;
        }

        // Make sure new node is connected to full node's old neighbour
        if (node->next)
        {
            node->next->previous = new;
        }
        new->next = node->next;

        // Connect new node to full node
        node->next = new;
        new->previous = node;

        // Check if new node is tail
        if (list->tail == node)
        {
            list->tail = new;
        }
    }

    // Check if existing items need to be shifted to make room
    if (((node == new ) && (index != node->used)) || (node != new))
    {
        // Number of bytes to be moved
        size_t bytes_to_move = ((node->used - index) - 1)
            * list->item_size_bytes;

        // Move last item to new location
        memcpy(new->data, NODE_DATA(list, node, index) + bytes_to_move, list->item_size_bytes);

        char *dest = NODE_DATA(list, node, index + 1);
        char *src = NODE_DATA(list, node, index);

        // Move remaining items to make room for new item
        memmove(dest, src, bytes_to_move);
    }

    // Copy item to target location
    memcpy(NODE_DATA(list, node, index), item, list->item_size_bytes);

    // Increment counters
    new->used += 1;
    list->num_items += 1;

    return ULIST_OK;
}

// Find a specific data item by index and fill out an access_params_t instance
static access_params_t *_find_item_by_index(ulist_t *list, unsigned long long index,
    access_params_t *access_params)
{
    ulist_node_t *node = NULL;
    size_t local_index;

    if ((NULL == access_params) || ((index + 1) > list->num_items))
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
    else if ((index + 1u) == list->num_items)
    {
        node = list->tail;
        local_index = list->tail->used - 1u;
    }
    // Somewhere in between
    else
    {
        node = list->head;
        unsigned long long total_items = 0u;

        while (NULL != node)
        {
            total_items += node->used;
            if (total_items >= (index + 1))
            {
                break;
            }

            node = node->next;
        }

        local_index = node->used - (total_items - index);
    }

    if (NULL == node)
    {
        return NULL;
    }

    access_params->node = (ulist_node_t *)node;
    access_params->local_index = local_index;
    return access_params;
}

static ulist_status_e _new_tail_item(ulist_t *list, void *item)
{
    access_params_t params = { .node=list->tail, .local_index=list->tail->used };

    // Tail node is full, create new
    if (list->tail->used == list->items_per_node)
    {
        ulist_node_t *new;
        if ((new = _new_node(list)) == NULL)
        {
            return ULIST_ERROR_MEM;
        }

        list->tail->next = new;
        new->previous = list->tail;
        list->tail = new;

        params.node = new;
        params.local_index = 0u;
        
    }

    return _add_item_to_node(list, params.node, params.local_index, item);
}

static int _check_write_index(ulist_t *list, unsigned long long index)
{
    return (0u == list->num_items) ? 0u == index : index <= list->num_items;
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
    if ((NULL == list) || (0u == item_size_bytes) || (0u == items_per_node))
    {
        return ULIST_INVALID_PARAM;
    }

    memset(list, 0, sizeof(ulist_t));
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

ulist_status_e ulist_insert_item(ulist_t *list, unsigned long long index, void *item)
{
    if ((NULL == list) || (NULL == list->tail) || (NULL == item))
    {
        return ULIST_INVALID_PARAM;
    }

    if (!_check_write_index(list, index))
    {
        return ULIST_INDEX_OUT_OF_RANGE;
    }

    // Add new item on the tail
    if (index == list->num_items)
    {
        return _new_tail_item(list, item);
    }

    access_params_t params;

    if (_find_item_by_index(list, index, &params) == NULL)
    {
        return ULIST_ERROR_INTERNAL;
    }

    return _add_item_to_node(list, params.node, params.local_index, item);
}

ulist_status_e ulist_append_item(ulist_t *list, void *item)
{
    if (NULL == list)
    {
        return ULIST_INVALID_PARAM;
    }

    return ulist_insert_item(list, list->num_items, item);
}

ulist_status_e ulist_get_item(ulist_t *list, unsigned long long index, void **item)
{
    if ((NULL == list) || (NULL == list->tail) || (NULL == item))
    {
        return ULIST_INVALID_PARAM;
    }

    if (index >= list->num_items)
    {
        return ULIST_INDEX_OUT_OF_RANGE;
    }

    access_params_t params;

    if (_find_item_by_index(list, index, &params) == NULL)
    {
        return ULIST_ERROR_INTERNAL;
    }

    *item = NODE_DATA(list, params.node, params.local_index);
    return ULIST_OK;
}
