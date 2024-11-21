/*
    Implement module column for tman project.
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <assert.h>
#include <string.h>

#include "tman.h"
#include "common.h"

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
    char id[20];
    struct column col;
};

struct coltab coltab[NCOLTAB];

// TODO: fill out this struct from config file
struct column columns[] = {
    { .mark = '*', .level = 0, .tag = "curr" },
    { .mark = '^', .level = 1, .tag = "prev" },
    { .mark = '+', .level = 2, .tag = "blog" },
    // user defined columns
    { .mark = '=', .level = 3, .tag = "revw" },
    { .mark = '>', .level = 4, .tag = "test" },
    // user defined columns
    { .mark = '-', .level = 5, .tag = "done" },
};

size_t ncolumns = sizeof(columns) / sizeof(columns[0]);


int column_loadids(char *env)
{
    DIR *dp;
    FILE *fp;
    int colfound;
    int colidx = 0;
    char fname[1000];
    char line[BUFSIZ];
    char key[100], val[100];
    struct dirent *dir;

    sprintf(fname, "%s/%s", tmanfs.task, env);
    if ((dp = opendir(fname)) == NULL) {
        fprintf(stderr, "could not open env dir %s\n", env);
    }

    while ((dir = readdir(dp)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0 || dir->d_type != DT_DIR)
            continue;

        // TODO:BEGIN move it to a function
        sprintf(fname, "%s/%s/%s/.tman/col", tmanfs.task, env, dir->d_name);
        if ((fp = fopen(fname, "r")) == NULL) {
            fprintf(stderr, "could not open task ID: %s\n", fname);
            continue;
        }

        fgets(line, BUFSIZ, fp);
        sscanf(line, "%*s : %s", coltab[colidx].col.tag);
        strcpy(coltab[colidx].id, dir->d_name);
        ++colidx;

        fclose(fp);
        // TODO:END move it to a function
    }
    return closedir(dp);
}

// TODO: save only specific ID, not them all
int column_saveids(char *env)
{
    DIR *dp;
    FILE *fp;
    int colfound;
    int colidx = 0;
    char fname[1000];
    char line[BUFSIZ];
    char key[100], val[100];
    struct dirent *dir;

    sprintf(fname, "%s/%s", tmanfs.task, env);
    if ((dp = opendir(fname)) == NULL) {
        fprintf(stderr, "could not open env dir %s\n", env);
    }

    for (int i = 0; i < NCOLTAB; ++i) {
        char *id = coltab[i].id;

        // TODO:BEGIN move it to a function
        sprintf(fname, "%s/%s/%s/.tman/col", tmanfs.task, env, id);
        if ((fp = fopen(fname, "w")) == NULL) {
            fprintf(stderr, "could not open task ID: %s\n", fname);
            continue;
        }

        fprintf(fp, "col : %s\n", coltab[colidx].col.tag);
        fclose(fp);
        // TODO:END move it to a function
    }
    return closedir(dp);
}

static int column_show()
{
    /* Ouput tasks separated by columns */
    for (int i = 0; i < ncolumns; ++i) {
        for (int k = 0; k < NCOLTAB; ++k) {
            if (strcmp(coltab[k].col.tag, columns[i].tag) == 0)
                printf("%s %s\n", columns[i].tag, coltab[k].id);
        }
    }
    printf("\n");
    return 0;
}

static int colmark(struct column *col, char *tag)
{
    for (int i = 0; i < ncolumns; ++i) {
        if (strcmp(columns[i].tag, tag) == 0) {
            strcpy(col->tag, columns[i].tag);
            col->mark = columns[i].mark;
            col->level = columns[i].level;
            return 0;
        }
    }
    return 1;
}

/* Load mark from task unit file.  */
static int colget(char *env, char *id)
{
    return 0;
}

/* Save mark to task unit file.  */
static int colset(void)
{
    return 0;
}
/* mark newly added task with blog column */
int column_mark(char *env, char *id)
{
    FILE *fp;
    char fname[BUFSIZ];

    sprintf(fname, "%s/%s/%s/.tman/col", tmanfs.task, env, id);
    if ((fp = fopen(fname, "w")) == NULL) {
        fprintf(stderr, "could not create column file %s\n", fname);
        return 1;
    }
    fprintf(fp, "col : %s\n", MARKDEF);
    return fclose(fp);
}

/* Add a new task to column (curr?) */
int column_addcid(char *id)
{
    // TODO: use a static varibale to hold curr and prev
    // at loading time.
    // TODO: check that current env is set

    for (int i = 0; i < NCOLTAB; ++i)
        if (strcmp(coltab[i].col.tag, MARKCURR) == 0 && strcmp(coltab[i].id, id) == 0) {
            fprintf(stderr, "%s: task id already marked as current\n", id);
            return 1;
        }

    /* First off, unset prev task ID. */
    for (int i = 0; i < NCOLTAB; ++i)
        if (strcmp(coltab[i].col.tag, MARKPREV) == 0)
            colmark(&coltab[i].col, MARKDEF);

    /* Second off, unset old current.  */
    for (int i = 0; i < NCOLTAB; ++i)
        if (strcmp(coltab[i].col.tag, MARKCURR) == 0)
            colmark(&coltab[i].col, MARKPREV);

    /* Third off, set new current.  */
    for (int i = 0; i < NCOLTAB; ++i)
        if (strcmp(coltab[i].id, id) == 0)
            colmark(&coltab[i].col, MARKCURR);

    return 0;
}

int column_delpid(void)
{
    /* Move current task to default column, i.e. blog.  */
    for (int i = 0; i < NCOLTAB; ++i)
        if (strcmp(coltab[i].col.tag, MARKPREV) == 0)
            colmark(&coltab[i].col, MARKDEF);
    return 0;
}

int column_delcid(void)
{
    /* Move current task to default column, i.e. blog.  */
    for (int i = 0; i < NCOLTAB; ++i)
        if (strcmp(coltab[i].col.tag, MARKCURR) == 0)
            colmark(&coltab[i].col, MARKDEF);
    /* Mark previous task as new current.  */
    for (int i = 0; i < NCOLTAB; ++i)
        if (strcmp(coltab[i].col.tag, MARKPREV) == 0)
            colmark(&coltab[i].col, MARKCURR);
    return 0;
}

int column_swapids(void)
{
    int currfound = 0, prevfound = 0;

    for (int i = 0; i < NCOLTAB; ++i) {
        if (strcmp(coltab[i].col.tag, MARKCURR) == 0)
            currfound = 1;
        else if (strcmp(coltab[i].col.tag, MARKPREV) == 0)
            prevfound = 1;
    }

    if (!(currfound && prevfound)) {
        printf("nothing to swap: curr or prev missing\n");
        return 0;
    }
    for (int i = 0; i < NCOLTAB; ++i) {
        if (strcmp(coltab[i].col.tag, MARKCURR) == 0)
            colmark(&coltab[i].col, MARKPREV);
        else if (strcmp(coltab[i].col.tag, MARKPREV) == 0)
            colmark(&coltab[i].col, MARKCURR);
    }
    return 0;
}

/* Move an existing task to new column.  */
int column_set(char *id, char *tag)
{
    // TOOD: what do if current or previous taks IDs passed
    for (int i = 0; i < NCOLTAB; ++i) {
        if (strcmp(coltab[i].id, id) == 0)
            return colmark(&coltab[i].col, tag);
    }
    return 1;
}


#include "state.h"

struct state column_getcenv()
{
}

int column_getpenv()
{
    return 0;
}
