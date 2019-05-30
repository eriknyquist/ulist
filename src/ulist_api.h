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
 * @return   ULIST_OK        if list instance was initialized successfully
 */
ulist_status_e ulist_create(ulist_t *list, size_t item_size_bytes, size_t items_per_node);

/**
 * Destroy an initialized list instance. Memory will leak if all created list
 * instances are not destroyed.
 *
 * @param    list            List instance to destroy
 *
 * @return   ULIST_OK        if list instance was destroyed successfully
 */
ulist_status_e ulist_destroy(ulist_t *list);

/**
 * Add an item to the end of a list.
 *
 * @param    list            List instance
 * @param    item            Pointer to item data to append
 *
 * @return   ULIST_OK        if item was appended successfully
 */
ulist_status_e ulist_append_item(ulist_t *list, void *item);

/**
 * Insert an item at a specific index in a list.
 *
 * @param    list            List instance
 * @param    index           List index to insert item at
 * @param    item            Pointer to item data to insert
 *
 * @return   ULIST_OK        if item was inserted successfully
 */
ulist_status_e ulist_insert_item(ulist_t *list, unsigned long long index, void *item);

/**
 * Fetch an item from a specific index in a list.
 *
 * @param    list            List instance
 * @param    index           List index of item to fetch
 * @param    item            Pointer to copy item data to
 *
 * @return   ULIST_OK        if item was fetched successfully
 */
ulist_status_e ulist_get_item(ulist_t *list, unsigned long long index, void *item);

/**
 * Fetch and remove an item from a specific index in a list.
 *
 * @param    list            List instance
 * @param    index           List index of item to pop
 * @param    item            Pointer to copy item data to
 *
 * @return   ULIST_OK        if item was popped successfully
 */
ulist_status_e ulist_pop_item(ulist_t *list, unsigned long long index, void *item);

#endif
