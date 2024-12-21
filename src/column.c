/*
    Implement module column for tman project.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>

#include "common.h"
#include "task.h"
#include "column.h"

char col[COLSIZ + 1];

struct column coltab[NCOLUMNS] = { /* user defined columns from config */
    { .prio = 0, .mark = '?', .tag = "uknw" },
    { .prio = 1, .mark = '*', .tag = "curr" },
    { .prio = 2, .mark = '^', .tag = "prev" },
    { .prio = 3, .mark = '+', .tag = "blog" },

// user define columns (gotta read 'em from config file
    { .prio = 4, .mark = '>', .tag = "revw" },
    { .prio = 5, .mark = '$', .tag = "test" },
    { .prio = 6, .mark = '!', .tag = "lock" },
// user define columns (gotta read 'em from config file

// FIXME: should be the last prio in table
    { .prio = 7, .mark = '-', .tag = "done" },
};

int column_exists(char *newcol)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(coltab); ++i)
        if (strncmp(newcol, coltab[i].tag, COLSIZ) == 0)
            return TRUE;
    return FALSE;
}

int column_add(char *env, char *id, char *newcol)
{
    FILE *fp;

    if ((fp = fopen(genpath_col(env, id), "w")) == NULL)
        return 1;
    fprintf(fp, "col : %s\n", newcol);
    return fclose(fp);
}

char *column_get(char *env, char *id)
{
    FILE *fp;

    if ((fp = fopen(genpath_col(env, id), "r")) == NULL)
        return NULL;
    else if (fscanf(fp, "col : %s\n", col) != NCOLITEM)
        return NULL;
    fclose(fp);
    return col;
}

// roach: should it delete col file or mark it with default column?
int column_del(char *env, char *id, char *newcol)
{
    return 0;
}

// Note: used only by tman_id_list(). Get rid of it in the future.
struct column column_getmark(char *env, char *id)
{
    int i;
    FILE *fp;
    char tag[TAGSIZ + 1];

    // NOTE: hotfix
    if ((fp = fopen(genpath_col(env, id), "r")) == NULL)
        return coltab[7];

    fscanf(fp, "%*s : %4s\n", tag);
    fclose(fp);

    for (i = 0; i < ARRAY_SIZE(coltab); ++i)
        if (strncmp(tag, coltab[i].tag, TAGSIZ) == 0)
            return coltab[i];
    return coltab[i];
}
