#include <stdlib.h>
#include <string.h>

#include "tree.h"

struct tree *tree_alloc(char *id, int colprio, char *desc)
{
    struct tree *node;

    if ((node = malloc(sizeof(struct tree))) == NULL)
        return NULL;

    // NOTE: the fix for somehow sets off uninited values
    memset(node, 0, sizeof(struct tree));

    // TODO: fucking wrappare. Add full support for user defined columns as well
    if (colprio == 1)
        node->mark = '*';
    else if (colprio == 2)
        node->mark = '^';
    else
        node->mark = '+';

    node->colprio = colprio;
    /*
       strncpy(node->id, id, IDSIZ);
       strncpy(node->desc, desc, DESCSIZ);
     */
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
