#include "server.h"

int is_malloc_ok(Line *node)
{
    if (node == NULL)
    {
        perror("head malloc!");
        exit(1);
    }
    return MALLOC_OK;
}

void init_list(Line **head)
{
    (*head) = (Line *)malloc(sizeof(Line));
    is_malloc_ok(*head);
    (*head)->next = NULL;
    return;
}
