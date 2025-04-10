#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "col.h"
#include "common.h"

struct tree *tree_alloc(char *id, int colprio, char *desc, char *pgnout)
{
    struct tree *node;

    if ((node = malloc(sizeof(struct tree))) == NULL)
        return NULL;

    // NOTE: somehow sets off uninited values
    memset(node, 0, sizeof(struct tree));

    node->mark = col_get2(colprio);
    node->colprio = colprio;
    strncpy(node->id, id, IDSIZ);
    strncpy(node->desc, desc, DESCSIZ);
    strncpy(node->pgnout, pgnout, PGNOUTSIZ);
    node->left = node->right = NULL;
    return node;
}

struct tree *tree_add(struct tree *parent, struct tree *node)
{
    if (parent == NULL)
        parent = node;
    else if (node->colprio <= parent->colprio)
        parent->left = tree_add(parent->left, node);
    else
        parent->right = tree_add(parent->right, node);
    return parent;
}

struct tree *tree_free(struct tree *parent)
{
    if (parent != NULL) {
        tree_free(parent->left);
        // free data if any in between recursive calls
        tree_free(parent->right);
        free(parent);
    }
    return NULL;
}

void tree_print(struct tree *parent)
{
    if (parent != NULL) {
        tree_print(parent->left);
        printf("-> %c %s [%s] '%s'\n", parent->mark, parent->id,
               parent->pgnout, parent->desc);
        tree_print(parent->right);
    }
}
