#include <stdlib.h>
#include <string.h>

#include "tree.h"
#include "common.h"

struct tree *tree_alloc(char *id, int col, char *desc, char *pgnout)
{
    struct tree *node;

    if ((node = malloc(sizeof(struct tree))) == NULL)
        return NULL;
    node->col = col;
    strncpy(node->id, id, IDSIZ);
    strncpy(node->desc, desc, IDSIZ);
    memset(node->pgnout, 0, PGNOUTSCSIZ); // TODO: define it too
    node->left = node->right = NULL;
    return node;
}

struct tree *tree_add(struct tree *parent, struct tree *node)
{
    if (node == NULL)
        ;

    if (parent == NULL)
        parent = node;
    else if (node->col <= parent->col)
        parent->left = tree_add(parent->left, node);
    else
        parent->right = tree_add(parent->right, node);
    return parent;
}

void tree_free(struct tree *parent)
{
    if (parent != NULL) {
        tree_free(parent->left);
        // free data if any in between recursive calls
        tree_free(parent->right);
        free(parent);
    }
}

void tree_print(struct tree *parent)
{
    if (parent != NULL) {
        tree_print(parent->left);
        printf("tree: %s %d '%s'\n", parent->id, parent->col, parent->desc);
        tree_print(parent->right);
    }
}
