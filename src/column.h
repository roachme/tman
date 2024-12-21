#ifndef TMAN_COLUMN_H
#define TMAN_COLUMN_H

#include "common.h"

#define NCOLITEM    1
#define MARKDEF     "blog"
#define MARKCURR    "curr"
#define MARKPREV    "prev"
#define MARKDONE    "done"

struct column {
    int prio;
    char mark;
    char env[ENVSIZ + 1];
    char tag[TAGSIZ + 1];
};

struct columns {
    int size;
    struct column column[NCOLUMNS];
};

int column_add(char *env, char *id, char *col);
int column_exists(char *col);
char *column_get(char *env, char *id);

// Note: used only by tman_id_list(). Do somethnig about it.
struct column column_getmark(char *env, char *id);

#endif
