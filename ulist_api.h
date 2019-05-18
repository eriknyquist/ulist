/*
 * list_api.h
 */
#ifndef ULIST_API_H
#define ULIST_API_H

#include <stdlib.h>

typedef enum {
    ULIST_OK,                 // Success
    ULIST_INVALID_PARAM,      // Invalid parameter passed to function
    ULIST_ALREADY_INIT,       // ulist_create called more than once for one list
	ULIST_INDEX_OUT_OF_RANGE, // Invalid list index provided
    ULIST_ERROR_MEM,          // Memory allocation failed
    ULIST_ERROR_INTERNAL,     // Unspecified internal error
} ulist_status_e;

typedef struct ulist_node ulist_node_t;

struct ulist_node {
    struct ulist_node *next;
    struct ulist_node *previous;
    size_t used;
    char data[];
};

typedef struct {
    ulist_node_t *head;
    ulist_node_t *tail;
    size_t item_size_bytes;
    size_t items_per_node;
    unsigned long long num_items;
} ulist_t;

ulist_status_e ulist_node_size_bytes(ulist_t *list, size_t *size_bytes);
ulist_status_e ulist_create(ulist_t *list, size_t item_size_bytes, size_t items_per_node);
ulist_status_e ulist_destroy(ulist_t *list);
ulist_status_e ulist_append_item(ulist_t *list, void *item);
ulist_status_e ulist_insert_item(ulist_t *list, unsigned long long index, void *item);
ulist_status_e ulist_get_item(ulist_t *list, unsigned long long index, void **item);

#endif
