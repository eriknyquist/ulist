/*
 * main.c
 */

#include <stdio.h>
#include "ulist_api.h"

#define ITEMS_PER_NODE (5u)

#define TESTLIST_SIZE (12u)

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
        int *item;
        err = ulist_get_item(&list, i, (void **)&item);
        if (err != ULIST_OK)
        {
            return err;
        }

        printf("%d\n", *item);
    }
}
