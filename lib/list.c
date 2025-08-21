#include <string.h>
#include <stdlib.h>

#include "list.h"

void list_free(tec_list_t * head)
{
    tec_list_t *p, *q;

    for (p = head; p != NULL; p = q) {
        q = p->next;
        free(p->name);
        free(p);
    }
}

tec_list_t *list_add(tec_list_t * head, char *id, int status)
{
    tec_list_t *node;

    if ((node = malloc(sizeof(tec_list_t))) == NULL)
        return NULL;
    else if ((node->name = strdup(id)) == NULL) {
        free(node);
        return NULL;
    }
    node->status = status;
    node->next = head;
    return node;
}
