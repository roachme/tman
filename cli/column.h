#ifndef TMAN_COLUMN_H
#define TMAN_COLUMN_H

// TODO: delete it all once support for user defined columns added.

struct column {
    char mark;
    char tag[10];
    char desc[80];
};

extern struct column g_colname[];
extern const int g_colname_size;

int column_exist(char *name);

#endif
