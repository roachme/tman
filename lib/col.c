/*
    Implement module column for tman project.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>

#include "common.h"
#include "col.h"

static char col[COLSIZ + 1];

struct column coltab[NCOLUMNS] = {
    /* system defined columns.  */
    {.prio = 0,.mark = '?',.col = "uknw"},
    {.prio = 1,.mark = '*',.col = "curr"},
    {.prio = 2,.mark = '^',.col = "prev"},
    {.prio = 3,.mark = '+',.col = "blog"},

// user define columns (gotta read 'em from config file
    {.prio = 4,.mark = '>',.col = "revw"},
    {.prio = 5,.mark = '$',.col = "test"},
    {.prio = 6,.mark = '!',.col = "lock"},
    {.prio = 7,.mark = '-',.col = "done"},
};

int col_ext(char *newcol)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(coltab); ++i)
        if (strncmp(newcol, coltab[i].col, COLSIZ) == 0)
            return TRUE;
    return FALSE;
}

int col_add(char *prj, char *id, char *newcol)
{
    FILE *fp;

    if ((fp = fopen(genpath_col(prj, id), "w")) == NULL)
        return 1;
    fprintf(fp, "col : %s\n", newcol);
    return fclose(fp);
}

char *col_get(char *prj, char *id)
{
    FILE *fp;

    if ((fp = fopen(genpath_col(prj, id), "r")) == NULL)
        return NULL;
    else if (fscanf(fp, "col : %s\n", col) != NCOLITEM) {
        fclose(fp);
        return NULL;
    }
    fclose(fp);
    return col;
}

char col_get2(int prio)
{
    for (int i = 0; i < ARRAY_SIZE(coltab); ++i)
        if (coltab[i].prio == prio)
            return coltab[i].mark;
    return coltab[7].mark;
}

int col_set(char *prj, char *id, char *col)
{
    FILE *fp;

    if ((fp = fopen(genpath_col(prj, id), "w")) == NULL) {
        fprintf(stderr, "err: setcol '%s'\n", id);
        return 1;
    }
    fprintf(fp, COLFMT, col);
    return fclose(fp);
}

int col_prio(char *col)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(coltab); ++i)
        if (strncmp(col, coltab[i].col, COLSIZ) == 0)
            return coltab[i].prio;
    return coltab[7].prio;
}

// roach: should it delete col file or mark it with default column?
int col_del(char *prj, char *id)
{
    return 0;
}

// Note: used only by tman_task_list(). Get rid of it in the future.
struct column col_getmark(char *prj, char *id)
{
    int i;
    FILE *fp;
    char tag[COLSIZ + 1];

    // NOTE: hotfix
    if ((fp = fopen(genpath_col(prj, id), "r")) == NULL)
        return coltab[7];

    fscanf(fp, "%*s : %4s\n", tag);
    fclose(fp);

    for (i = 0; i < ARRAY_SIZE(coltab); ++i)
        if (strncmp(tag, coltab[i].col, COLSIZ) == 0)
            return coltab[i];
    return coltab[0];
}
