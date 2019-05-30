/*
 * main.c
 */

#include <stdio.h>
#include "ulist_api.h"

#define ITEMS_PER_NODE (32u)

#define TESTLIST_SIZE (1000000u)

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

int main(int argc, char *argv[])
{
    ulist_status_e err;
    ulist_t list;

    err = ulist_create(&list, sizeof(int), ITEMS_PER_NODE);
    if (err != ULIST_OK)
    {
        return err;
    }

    for (int i = 0; i < TESTLIST_SIZE; i++)
    {
        err = ulist_append_item(&list, &i);
        if (err != ULIST_OK)
        {
            return err;
        }
    }

    for (int i = 0; i < TESTLIST_SIZE; i++)
    {
        int val;

        err = ulist_get_item(&list, i, &val);
        if (err != ULIST_OK)
        {
            return err;
        }
    }

    if ((err = ulist_destroy(&list)) != ULIST_OK)
    {
        return err;
    }

    return 0;
}
