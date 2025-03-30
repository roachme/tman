#ifndef LIBTMAN_TREE_H
#define LIBTMAN_TREE_H

#include "common.h"
#include "task.h"

struct tree {
    int mark;
    int colprio;
    char id[IDSIZ + 1];
    char desc[DESCSIZ + 1];
    char pgnout[PGNOUTSIZ + 1];
    struct tree *left;
    struct tree *right;
};

struct tree *tree_alloc(char *id, int col, char *desc, char *pgnout);
struct tree *tree_add(struct tree *parent, struct tree *node);
struct tree *tree_free(struct tree *parent);
void tree_print(struct tree *parent);

#endif
