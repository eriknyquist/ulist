/**
 * @file   ulist_api.h
 * @author Erik Nyquist
 * @brief  Unrolled linked list API
 */
#ifndef ULIST_API_H
#define ULIST_API_H

#include <stdlib.h>


/* Status codes for list operations */
typedef enum {
    ULIST_OK,                 // Success
    ULIST_END,                // Reached end of list during iteration
    ULIST_INVALID_PARAM,      // Invalid parameter passed to function
    ULIST_ALREADY_DESTROYED,  // ulist_destroy called again on a destroyed list
    ULIST_INDEX_OUT_OF_RANGE, // Invalid list index provided
    ULIST_ERROR_MEM,          // Memory allocation failed
    ULIST_ERROR_INTERNAL,     // Unspecified internal error
} ulist_status_e;


/* Single node in a ulist */
typedef struct ulist_node ulist_node_t;

struct ulist_node {
    struct ulist_node *next;
    struct ulist_node *previous;
    size_t used;
    char data[];
};


/* Single ulist instance */
typedef struct {
    ulist_node_t *head;
    ulist_node_t *tail;
    size_t item_size_bytes;
    size_t items_per_node;
    unsigned long long num_items;
    unsigned long long nodes;

    // Iteration parameters
    ulist_node_t *current;
    size_t local_index;
    unsigned long long index;
} ulist_t;


/**
 * Query the size of a node for a specific list.
 *
 * @param    list        List instance
 * @param    size_bytes  Pointer to write node size to
 *
 * @return   ULIST_OK
 */
ulist_status_e ulist_node_size_bytes(ulist_t *list, size_t *size_bytes);


/**
 * Initialize a list instance.
 *
 * @param    list            Uninitialized list structure to initialize
 * @param    item_size_bytes Size of a single list item in bytes
 * @Param    items_per_node  Number of items that each list node should hold
 *
 * @return   ULIST_OK        If list instance was initialized successfully
 */
ulist_status_e ulist_create(ulist_t *list, size_t item_size_bytes,
    size_t items_per_node);


/**
 * Destroy an initialized list instance. Memory will leak if all created list
 * instances are not destroyed.
 *
 * @param    list            List instance to destroy
 *
 * @return   ULIST_OK        If list instance was destroyed successfully
 */
ulist_status_e ulist_destroy(ulist_t *list);


/**
 * Add an item to the end of a list.
 *
 * @param    list            List instance
 * @param    item            Pointer to item data to append
 *
 * @return   ULIST_OK        If item was appended successfully
 */
ulist_status_e ulist_append_item(ulist_t *list, void *item);


/**
 * Insert an item at a specific index in a list.
 *
 * @param    list            List instance
 * @param    index           List index to insert item at
 * @param    item            Pointer to item data to insert
 *
 * @return   ULIST_OK        If item was inserted successfully
 */
ulist_status_e ulist_insert_item(ulist_t *list, unsigned long long index,
    void *item);


/**
 * Fetch an item from a specific index in a list.
 *
 * @param    list            List instance
 * @param    index           List index of item to fetch
 * @param    item            Pointer to copy item data to
 *
 * @return   ULIST_OK        If item was fetched successfully
 */
ulist_status_e ulist_get_item(ulist_t *list, unsigned long long index,
    void *item);


/**
 * Fetch the next item in the list, starting from the head item or from the
 * item at the iteration start index (if set). This is a much faster option for
 * iterating over items in a list when compared to #ulist_get_item, since
 * #ulist_get_item has to crawl through the list to find the right node for each
 * call. #ulist_get_next_item instead keeps references to the current node and
 * item so we can just jump to the next node when a subsequent call requires it.
 *
 * @param    list            List instance
 * @param    item            Pointer to copy item pointer to. Note that this
 *                           pointer may become invalid if items are added to or
 *                           removed from the list.
 *
 * @return   ULIST_OK        If next item was fetched successfully, or ULIST_END
 *                           if the end of the list has been reached
 */
ulist_status_e ulist_get_next_item(ulist_t *list, void **item);


/**
 * Fetch a pointer to the previous item in the list, starting from the tail
 * item or from the item at the iteration start index (if set). This is a much
 * faster option for iterating over items in a list when compared to
 * #ulist_get_item, since #ulist_get_item has to crawl through the list to find
 * the right node for each call. #ulist_get_previous_item instead keeps
 * references to the current node and item so we can just jump to the previous
 * node when a subsequent call requires it.
 *
 * @param    list            List instance
 * @param    item            Pointer to copy item pointer to. Note that this
 *                           pointer may become invalid if items are added to or
 *                           removed from the list.
 *
 * @return   ULIST_OK        If next item was fetched successfully, or ULIST_END
 *                           if the beginning of the list has been reached
 */
ulist_status_e ulist_get_previous_item(ulist_t *list, void **item);


/**
 * Set the item index to start the next iteration from. The item at this index
 * will be fetched by the next call to #ulist_get_next_item or
 * #ulist_get_previous_item
 *
 * @param    list            List instance
 * @param    index           List index of item to fetch
 *
 * @return   ULIST_OK        If the iteration start index was set successfully
 */
ulist_status_e ulist_set_iteration_start_index(ulist_t *list,
    unsigned long long index);


/**
 * Fetch and remove an item from a specific index in a list.
 *
 * @param    list            List instance
 * @param    index           List index of item to pop
 * @param    item            Pointer to copy item data to
 *
 * @return   ULIST_OK        If item was popped successfully
 */
ulist_status_e ulist_pop_item(ulist_t *list, unsigned long long index,
    void *item);


#endif
