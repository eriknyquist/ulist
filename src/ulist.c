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
static ulist_node_t *_alloc_new_node(ulist_t *list)
{
    ulist_node_t *node;

    if ((node = malloc(NODE_ALLOC_SIZE(list))) == NULL)
    {
        return NULL;
    }

    memset(node, 0, sizeof(ulist_node_t));
    return node;
}

static void _add_to_nonfull_node(ulist_t *list, access_params_t *params,
    void *item)
{
    // Target location to copy new item to
    char *target = NODE_DATA(list, params->node, params->local_index);

    // Check if existing items need to be shifted to make room
    if (params->local_index != params->node->used)
    {
        // Number of bytes to be moved
        size_t bytes_to_move = (params->node->used - params->local_index)
            * list->item_size_bytes;

        char *dest = target + list->item_size_bytes;

        // Move items to make room for new items
        memmove(dest, target, bytes_to_move);
    }

    // Copy item to target location
    memcpy(target, item, list->item_size_bytes);
    params->node->used += 1u;
}

// Add a new node after the given node, and move half of the full node's
// contents into the new node.
static ulist_node_t *_add_to_full_node(ulist_t *list, access_params_t *params,
    void *item)
{
    ulist_node_t *new = NULL;

    if ((new = _alloc_new_node(list)) == NULL)
    {
        return NULL;
    }

    // Make sure new node is connected to full node's old neighbour
    if (params->node->next)
    {
        params->node->next->previous = new;
    }
    new->next = params->node->next;

    // Connect new node to full node
    params->node->next = new;
    new->previous = params->node;

    // Check if new node is tail
    if (list->tail == params->node)
    {
        list->tail = new;
    }

    size_t items_to_move = params->node->used / 2u;
    size_t bytes_to_move = items_to_move * list->item_size_bytes;
    size_t move_start_index = params->node->used - items_to_move;

    // Move half of the full node's items to the new node
    memcpy(new->data,
        NODE_DATA(list, params->node, move_start_index),
        bytes_to_move);

    new->used += items_to_move;
    params->node->used -= items_to_move;

    if (params->local_index >= move_start_index)
    {
        // item must be inserted in new node
        size_t new_index = params->local_index - move_start_index;
        params->node = new;
        params->local_index = new_index;
    }

    _add_to_nonfull_node(list, params, item);

    return new;
}

// Add item to node, creating a new node if required
static ulist_status_e _insert_item(ulist_t *list, access_params_t *params,
    void *item)
{
    ulist_node_t *new = params->node;

    // Current node is full, create a new one
    if (params->node->used == list->items_per_node)
    {
        if ((new = _add_to_full_node(list, params, item)) == NULL)
        {
            return ULIST_ERROR_MEM;
        }
    }
    else
    {
        _add_to_nonfull_node(list, params, item);
    }

    list->num_items += 1u;

    return ULIST_OK;
}

// Find a specific data item by index and fill out an access_params_t instance
static access_params_t *_find_item_by_index(ulist_t *list, unsigned long long index,
    access_params_t *access_params)
{
    if ((NULL == access_params) || ((index + 1) > list->num_items))
    {
        return NULL;
    }

    ulist_node_t *node = list->head;
    unsigned long long item_count = 0u;
    size_t local_index;

    // Loop through nodes, incrementing item count until we reach target item
    while (NULL != node)
    {
        item_count += node->used;
        if (item_count >= (index + 1))
        {
            // Found/exceeded target item
            break;
        }

        node = node->next;
    }

    // Item index within node
    local_index = node->used - (item_count - index);

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
        if ((new = _alloc_new_node(list)) == NULL)
        {
            return ULIST_ERROR_MEM;
        }

        list->tail->next = new;
        new->previous = list->tail;
        list->tail = new;

        params.node = new;
        params.local_index = 0u;

    }

    return _insert_item(list, &params, item);
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

    if ((list->head = _alloc_new_node(list)) == NULL)
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

    return _insert_item(list, &params, item);
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
