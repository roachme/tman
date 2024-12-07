#ifndef TMAN_COLUMN_H
#define TMAN_COLUMN_H

#include "env.h"
#include "common.h"

// Gotta define 4 default columns: curr, prev, blog, done

#define MARKDEF     "blog"
#define MARKCURR    "curr"
#define MARKPREV    "prev"


struct column {
    int prio;
    char mark;
    char env[ENVSIZ];
    char tag[TAGSIZ+1];
};

struct taskid {
    int isset;
    char id[IDSIZ+1];
    struct column col;
};

struct taskids {
    int idx;
    struct taskid ids[NTASKS];
};

struct columns {
    int size;
    struct column column[NCOLUMNS];
};

int column_show(void);
int column_moveid(char *id, char *tag);
struct column column_getmark(char *id);

int column_init();
int column_getids(void);        /* get all task IDs in environment */
int column_markid(char *id);    /* add a new task ID's col file */

char *column_getcid();
char *column_getpid();
int column_delcid();
int column_delpid();
int column_swapid();
int column_addcid(char *id);
int column_delspec(char *id);

int column_envinit();
char *column_getcenv();
char *column_getpenv();
int column_delcenv();
int column_delpenv();
int column_swapenv();
int column_addcenv(char *env);

#endif
