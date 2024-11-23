#ifndef TMAN_COLUMN_H
#define TMAN_COLUMN_H

#include "common.h"

// Gotta define 4 default columns: curr, prev, blog, done

#define CENV        0
#define PENV        1
#define MARKDEF     "blog"
#define MARKCURR    "curr"
#define MARKPREV    "prev"

#define TRUE        1
#define FALSE       0


#define NENV        2  /* number of 'special' environments */
#define NTASKS      50 /* number of tasks per environment */
#define NCOLUMNS    10 /* number of columns per environment */

struct column {
    int prio;
    char mark;
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

struct env {
    char curr[IDSIZ+1];
    char prev[IDSIZ+1];
    char name[ENVSIZ+1];
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

int column_envinit();
char *column_getcenv();
char *column_getpenv();
int column_delcenv();
int column_delpenv();
int column_swapenv();
int column_addcenv(char *env);

#endif
