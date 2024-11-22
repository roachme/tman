#ifndef TMAN_COLUMN_H
#define TMAN_COLUMN_H


/* Limits */
#define NCOLTAB         20 /* Number of task per environment */
#define NCOLUMNS        10 /* Number of columns per environment, 3 defaults included */

#define MAXCOLTAB       20
#define TAGSIZ          4
#define MARKDEF         "blog"
#define MARKCURR        "curr"
#define MARKPREV        "prev"


struct column {
    char mark;
    int level;
    char tag[TAGSIZ + 1];
};

struct coltab {
    int isset;
    char id[20];
    struct column col;
};


int column_loadids(char *env);
int column_saveids(char *env);

int column_init(char *env);
int column_delpid(void);
int column_delcid(void);
int column_swapids(void);
int column_mark(char *env, char *id); /* mark newly added task with blog column */
int column_addcid(char *id);

#endif
