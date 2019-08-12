/**
 * @file   ulist.c
 * @author Erik Nyquist
 * @brief  Unrolled linked list implementation
 */
#include <string.h>
#include "ulist_api.h"

#define MIN_ITEMS_PER_NODE (2u)

#define GREEDY (1u)
#define NOT_GREEDY (0u)

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

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
    list->nodes += 1;
    return node;
}


/* Move items from src to dest until the number of items in dest has reached
 * more than half. dest and src are expected to be connected. */
static void _balance_nodes(ulist_t *list, ulist_node_t *dest,
        ulist_node_t *src, unsigned greedy)
{
    size_t items_to_move;

    if (greedy && ((src->used + dest->used) <= list->items_per_node))
    {
        // Data from both nodes can fit into one node
        items_to_move = src->used;

    }
    else
    {
        size_t half_items = list->items_per_node / 2u;
        size_t items_needed = half_items - dest->used;
        items_to_move = MIN(items_needed, src->used);
    }

    size_t bytes_to_move = items_to_move * list->item_size_bytes;

    // Direction of copying
    unsigned head_to_tail = (src->next == dest) ? 1u : 0u;

    if (head_to_tail)
    {
        if (dest->used > 0u)
        {
            // Existing data in dest, shift it to make room
            size_t dest_size = dest->used * list->item_size_bytes;
            memmove(dest->data + bytes_to_move, dest->data, dest_size);
        }

        // Move data from src to dest
        size_t src_index = src->used - items_to_move;
        memcpy(dest->data, NODE_DATA(list, src, src_index), bytes_to_move);
    }
    else
    {
        // Move data from src to dest
        memcpy(NODE_DATA(list, dest, dest->used), src->data, bytes_to_move);

        if (items_to_move < src->used)
        {
            // Remaining data in src, move it back to cover the free space
            memmove(
                src->data,
                src->data + bytes_to_move,
                (src->used - items_to_move) * list->item_size_bytes);
        }
    }

    dest->used += items_to_move;
    src->used -= items_to_move;
}


// Add item to a node that has space remaining -- no node allocation required
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


/* Add a new item when the target node is full -- allocate a new node, and move
 * half of the target node's contents into the new node. */
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

    _balance_nodes(list, new, params->node, NOT_GREEDY);

    if (params->local_index > params->node->used)
    {
        // item must be inserted in new node
        size_t new_index = params->local_index - params->node->used;
        params->node = new;
        params->local_index = new_index;
    }

    _add_to_nonfull_node(list, params, item);

    return new;
}


// Add item to node at a specific index, creating a new node if required
static ulist_status_e _insert_item(ulist_t *list, access_params_t *params,
    void *item)
{
    ulist_node_t *new = params->node;

    // If item is to be added to index of 0 of this node, and the previous node
    // is not full, we can avoid some operations and just add the item at the
    // end of the previous node
    if ((params->local_index == 0u) && params->node->previous
         && (params->node->previous->used < list->items_per_node))
    {
        params->local_index = params->node->previous->used;
        params->node = params->node->previous;
        _add_to_nonfull_node(list, params, item);
    }
    // Current node is full, create a new one
    else if (params->node->used == list->items_per_node)
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


// Free an empty node and connect the nodes on either side of it
static void _delete_node(ulist_t *list, ulist_node_t *node)
{
    if (node->next)
    {
        node->next->previous = node->previous;
    }

    if (node->previous)
    {
        node->previous->next = node->next;
    }

    if (list->head == node)
    {
        list->head = node->next;
    }
    else if (list->tail == node)
    {
        list->tail = node->previous;
    }

    free(node);
    list->nodes -= 1u;
}


/* Remove an item from the list. If the deleted item was the last one in the
 * node, then the empty node will be freed. */
static void _remove_item(ulist_t *list, access_params_t *params)
{
    if (params->local_index != (params->node->used - 1u))
    {
        // Need to move some items into the freed space
        size_t items_to_move = (params->node->used - 1u) - params->local_index;
        size_t bytes_to_move = items_to_move * list->item_size_bytes;

        memmove(
            NODE_DATA(list, params->node, params->local_index),
            NODE_DATA(list, params->node, params->local_index + 1u),
            bytes_to_move);
    }

    params->node->used -= 1u;
    list->num_items -= 1u;
    size_t half_items = list->items_per_node / 2u;

    if (params->node->used > half_items)
    {
        // Node is over half full, nothing else to do
        return;
    }

    ulist_node_t *src_node = params->node->next;

    if ((NULL == params->node->next) && (NULL == params->node->previous))
    {
        // Only node in the list, nothing else to do
        return;
    }
    else if (NULL == params->node->next)
    {
        // Node is tail
        src_node = params->node->previous;
    }

    // Move items from source node into current node
    _balance_nodes(list, params->node, src_node, GREEDY);

    if (0u == src_node->used)
    {
        // Source node is empty
        _delete_node(list, src_node);
    }
}


// Find an item by traversing the list from the head node
static void _forward_crawl(ulist_t *list, unsigned long long index,
    access_params_t *params)
{
    ulist_node_t *node = list->head;
    unsigned long long item_count = 0u;

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
    params->local_index = node->used - (item_count - index);
    params->node = node;
}


// Find an item by traversing the list from the tail node
static void _backward_crawl(ulist_t *list, unsigned long long index,
    access_params_t *params)
{
    ulist_node_t *node = list->tail;
    unsigned long long item_count = list->num_items;

    // Loop through nodes, incrementing item count until we reach target item
    while (NULL != node)
    {
        item_count -= node->used;
        if (item_count <= index)
        {
            // Found/exceeded target item
            break;
        }

        node = node->previous;
    }

    // Item index within node
    params->local_index = index - item_count;
    params->node = node;
}


// Find a specific data item by index and fill out an access_params_t instance
static access_params_t *_find_item_by_index(ulist_t *list, unsigned long long index,
    access_params_t *access_params)
{
    if ((NULL == access_params) || ((index + 1) > list->num_items))
    {
        return NULL;
    }

    size_t avg_items_per_node = list->num_items / list->nodes;
    size_t roughly_half = (list->nodes / 2u) * avg_items_per_node;

    if (index <= roughly_half)
    {
        _forward_crawl(list, index, access_params);
    }
    else
    {
        _backward_crawl(list, index, access_params);
    }

    return access_params;
}


/* Special case for tail item-- instead of handling this in ulist_insert_item,
 * we can avoid a lot of checks and do things a bit quicker in this function */
static ulist_status_e _new_tail_item(ulist_t *list, void *item)
{
    access_params_t params = {.node=list->tail, .local_index=list->tail->used};

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

    _add_to_nonfull_node(list, &params, item);
    list->num_items += 1;
    return ULIST_OK;
}

static int _check_write_index(ulist_t *list, unsigned long long index)
{
    return (0u == list->num_items) ? 0u == index : index <= list->num_items;
}


/**
 * @see ulist_api.h
 */
ulist_status_e ulist_node_size_bytes(ulist_t *list, size_t *size_bytes)
{
    if ((NULL == list) || (NULL == size_bytes))
    {
        return ULIST_INVALID_PARAM;
    }

    *size_bytes = NODE_ALLOC_SIZE(list);

    return ULIST_OK;
}


/**
 * @see ulist_api.h
 */
ulist_status_e ulist_create(ulist_t *list, size_t item_size_bytes,
    size_t items_per_node)
{
    if ((NULL == list) || (0u == item_size_bytes) || (0u == items_per_node))
    {
        return ULIST_INVALID_PARAM;
    }

    if (items_per_node < MIN_ITEMS_PER_NODE)
    {
        return ULIST_INVALID_PARAM;
    }

    memset(list, 0, sizeof(ulist_t));
    list->item_size_bytes = item_size_bytes;
    list->items_per_node = items_per_node;
    list->current = NULL;

    if ((list->head = _alloc_new_node(list)) == NULL)
    {
        return ULIST_ERROR_MEM;
    }

    list->tail = list->head;
    return ULIST_OK;
}


/**
 * @see ulist_api.h
 */
ulist_status_e ulist_destroy(ulist_t *list)
{
    ulist_node_t *node, *old;
    if (NULL == list)
    {
        return ULIST_INVALID_PARAM;
    }

    if (NULL == list->head)
    {
        return ULIST_ALREADY_DESTROYED;
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


/**
 * @see ulist_api.h
 */
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

    // Special case for index of list->num_items, call _new_tail_item
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


/**
 * @see ulist_api.h
 */
ulist_status_e ulist_append_item(ulist_t *list, void *item)
{
    if ((NULL == list) || (NULL == item))
    {
        return ULIST_INVALID_PARAM;
    }

    return _new_tail_item(list, item);
}


/**
 * @see ulist_api.h
 */
ulist_status_e ulist_get_item(ulist_t *list, unsigned long long index,
    void *item)
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

    void *data = NODE_DATA(list, params.node, params.local_index);
    memcpy(item, data, list->item_size_bytes);

    return ULIST_OK;
}


/**
 * @see ulist_api.h
 */
ulist_status_e ulist_get_next_item(ulist_t *list, void **item)
{
    if ((NULL == list) || (NULL == item))
    {
        return ULIST_INVALID_PARAM;
    }

    // No iteration start index set-- start at the head
    if (NULL == list->current)
    {
        list->current = list->head;
        list->local_index = 0u;
    }

    // Reached the end of this node-- jump to the next one
    if (list->local_index == list->current->used)
    {
        // No more items
        if (NULL == list->current->next)
        {
            list->current = NULL;
            return ULIST_END;
        }

        list->current = list->current->next;
        list->local_index = 0u;
    }

    *item = NODE_DATA(list, list->current, list->local_index);
    list->local_index += 1u;

    return ULIST_OK;
}


/**
 * @see ulist_api.h
 */
ulist_status_e ulist_get_previous_item(ulist_t *list, void **item)
{
    static int _end_reached = 0;

    if ((NULL == list) || (NULL == item))
    {
        return ULIST_INVALID_PARAM;
    }

    if (1 == _end_reached)
    {
        _end_reached = 0;
        return ULIST_END;
    }

    // No iteration start index set-- start at the tail
    if (NULL == list->current)
    {
        // List is empty
        if (0u == list->tail->used)
        {
            return ULIST_END;
        }

        list->current = list->tail;
        list->local_index = list->tail->used - 1u;
    }

    *item = NODE_DATA(list, list->current, list->local_index);

    // Reached the end of this node-- jump to the previous one
    if (0u == list->local_index)
    {
        // No more items
        if (NULL == list->current->previous)
        {
            list->current = NULL;
            _end_reached = 1;
            return ULIST_OK;
        }

        list->current = list->current->previous;
        list->local_index = list->current->used - 1u;
    }
    else
    {
        list->local_index -= 1u;
    }

    return ULIST_OK;
}


/**
 * @see ulist_api.h
 */
ulist_status_e ulist_set_iteration_start_index(ulist_t *list,
    unsigned long long index)
{
    if (NULL == list)
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

    list->local_index = params.local_index;
    list->index = index;
    list->current = params.node;
    return ULIST_OK;
}


/**
 * @see ulist_api.h
 */
ulist_status_e ulist_pop_item(ulist_t *list, unsigned long long index, void *item)
{
    if ((NULL == list) || (NULL == list->tail))
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

    if (NULL != item)
    {
        void *data = NODE_DATA(list, params.node, params.local_index);
        memcpy(item, data, list->item_size_bytes);
    }

    _remove_item(list, &params);
    return ULIST_OK;
}
