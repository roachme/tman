#ifndef LIBTEC_TREE_H
#define LIBTEC_TREE_H

struct tree {
    int mark;
    int status;                 /* error code in case of invalid value */
    int colprio;
    char *id;
    char *desc;
    struct tree *left;
    struct tree *right;
};

struct tree *tree_alloc(char *id, int col, char *desc);
struct tree *tree_add(struct tree *parent, struct tree *node);
struct tree *tree_free(struct tree *parent);
void tree_print(struct tree *parent);

#endif
