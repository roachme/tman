#ifndef TMAN_COLUMN_H
#define TMAN_COLUMN_H

#include "common.h"

#define COLSIZ      10
#define NCOLITEM    1

#define COLFMT     "col : %s\n"
#define COLCURR    "curr"
#define COLPREV    "prev"
#define COLBLOG    "blog"
#define COLDONE    "done"

struct column {
    int prio;
    char mark;
    char env[ENVSIZ + 1];
    char col[COLSIZ + 1];
};

struct columns {
    int size;
    struct column column[NCOLUMNS];
};

int col_add(char *env, char *id, char *col);
int col_ext(char *col);
int col_prio(char *col);
char *col_get(char *env, char *id);
char col_get2(int prio);
int col_set(char *env, char *id, char *col);
int col_del(char *env, char *id);

// Note: used only by tman_id_list(). Do somethnig about it.
struct column col_getmark(char *env, char *id);

#endif
