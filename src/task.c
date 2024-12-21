#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "tman.h"
#include "task.h"
#include "column.h"

#define COLSIZ      10

#define TRUE        1
#define FALSE       0

#ifdef MYTEST
static char *base = "/home/roach/trash/tman";
#endif
static char *base = tmanfs.base;

static char coltag[COLSIZ + 1];
static char pathname[PATHSIZ + 1];
static char curr[IDSIZ + 1], prev[IDSIZ + 1];


static int load_toggles(char *env);
static int save_toggle(char *env, char *id, char *col);

#ifdef MYTEST
static int ISDIR(char *fname)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "test -d %s", fname);
    return system(cmd) == EXIT_SUCCESS;
}
#endif

static char *genpath(char *env)
{
    sprintf(pathname, "%s/%s", base, env);
    return pathname;
}

static char *col_genpath(char *env, char *id)
{
    sprintf(pathname, "%s/%s/%s/.tman/col", base, env, id);
    return pathname;
}

static int _fill_col_file(char *env, char *id, char *col)
{
    FILE *fp;

    if ((fp = fopen(col_genpath(env, id), "w")) == NULL)
        return 1;
    fprintf(fp, "col : %s\n", MARKBLOG);
    return fclose(fp);
}


static int reset_toggles()
{
    curr[0] = '\0';
    prev[0] = '\0';
    return 0;
}

static int _movecurr(char *env, char *col)
{
    if (strncmp(col, MARKCURR, COLSIZ) == 0)
        return 0; // do nothing
    else if (strncmp(col, MARKPREV, COLSIZ) == 0) {
        fprintf(stderr, "swap not allowed this way\n");
        return 1;
    }
    save_toggle(env, curr, col);
    if (prev[0] != '\0')
        save_toggle(env, prev, MARKCURR);
    reset_toggles();
    return 0;
}

static int _moveprev(char *env, char *col)
{
    if (strncmp(col, MARKPREV, COLSIZ) == 0)
        return 0; // do nothing
    else if (strncmp(col, MARKCURR, COLSIZ) == 0) {
        // TODO: should be legal.
        fprintf(stderr, "cant move prev to curr\n");
        return 1;
    }

    save_toggle(env, prev, col);
    reset_toggles();
    return 0;
}

static int _movetask(char *env, char *id, char *col)
{
    if (strncmp(col, MARKPREV, COLSIZ) == 0) {
        fprintf(stderr, "cant move task to prev this way\n");
        return 1;
    }

    if (strncmp(col, MARKCURR, COLSIZ) == 0 && curr[0] != '\0') {
        fprintf(stderr, "_movetask: old curr '%s'\n", curr);
        save_toggle(env, curr, MARKPREV);
        if (prev[0] != '\0')
            save_toggle(env, prev, MARKBLOG);
        save_toggle(env, id, MARKCURR);
    } else {
        fprintf(stderr, "_movetask: rest: new curr '%s'\n", id);
        save_toggle(env, id, col);
    }
    reset_toggles();
    return 0;
}

// TODO: delete `char *id' from API
static int _delete_curr(char *env)
{
    save_toggle(env, curr, MARKBLOG);
    if (prev[0] != '\0')
        save_toggle(env, prev, MARKCURR);
    return reset_toggles();
}

// TODO: delete `char *id' from API
static int _delete_prev(char *env)
{
    save_toggle(env, prev, MARKBLOG);
    return reset_toggles();
}

static int save_toggle(char *env, char *id, char *col)
{
    FILE *fp;

    if ((fp = fopen(col_genpath(env, id), "w")) == NULL)
        return FALSE;

    fprintf(fp, "col : %s\n", col);
    return fclose(fp) == 0;
}

static char *getcol(char *env, char *id)
{
    FILE *fp;
    char line[BUFSIZ + 1];
    char *tok;

    if ((fp = fopen(col_genpath(env, id), "r")) == NULL) {
        fprintf(stderr, "could not open col file\n");
        return NULL;
    }

    fgets(line, BUFSIZ, fp);
    //printf("col: '%s'\n", line);
    tok = strtok(line, " :\n");
    tok = strtok(NULL, " :\n");
    //printf("col: '%s'\n", tok);

    fclose(fp);
    return strncpy(coltag, tok, COLSIZ);
}

static int load_toggles(char *env)
{
    DIR *dir;
    char *id, *col;
    struct dirent *ent;

    if ((dir = opendir(genpath(env))) == NULL) {
        fprintf(stderr, "could not open env dir: %s\n", genpath(env));
        return FALSE;
    }

    while ((ent = readdir(dir)) != NULL) {
        id = ent->d_name;
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if ((col = getcol(env, id)) == NULL) {
            fprintf(stderr, "err: could not get taks unit: %s\n", id);
            continue;
        }

        if (strncmp(col, MARKCURR, COLSIZ) == 0)
            strncpy(curr, id, IDSIZ);
        else if (strncmp(col, MARKPREV, COLSIZ) == 0)
            strncpy(prev, id, IDSIZ);
    }
    closedir(dir);
    return TRUE;
}

int task_exists(char *env, char *id)
{
    char pathname[PATHSIZ + 1];
    sprintf(pathname, "%s/%s/%s", tmanfs.base, env, id);
    return ISDIR(pathname);
}

int task_add(char *env, char *id, char *col)
{
    char *newcol = col != NULL ? col : MARKCURR;

    if (task_exists(env, id) == FALSE)
        return 1;
    else if (column_exists(col) == FALSE) {
        fprintf(stderr, "column NOT exist: %s\n", col);
        return 1;
    }
    else if (_fill_col_file(env, id, MARKCURR)) {
        fprintf(stderr, "_fill_col_file: error\n");
        return 1;
    }
    return task_move(env, id, col);
}

int task_del(char *env, char *id)
{
    if (task_exists(env, id) == FALSE)
        return 1;
    else if (load_toggles(env) == FALSE)
        return 1;

    if (strncmp(id, curr, IDSIZ) == 0)
        return _delete_curr(env);
    return _delete_prev(env);
}

int task_move(char *env, char *id, char *col)
{
    fprintf(stderr, "task_move: start\n");

    if (task_exists(env, id) == FALSE) {
        fprintf(stderr, "task does NOT exist: %s\n", id);
        return 1;
    }
    else if (column_exists(col) == FALSE) {
        fprintf(stderr, "column NOT exist: %s\n", col);
        return 1;
    }
    else if (load_toggles(env) == FALSE) {
        fprintf(stderr, "task_move: could not load_toggles\n");
        return 1;
    }

    if (strncmp(id, curr, IDSIZ) == 0) {
        fprintf(stderr, "task_move: _movecurr\n");
        return _movecurr(env, col);
    }
    else if (strncmp(id, prev, IDSIZ) == 0) {
        fprintf(stderr, "task_move: _moveprev\n");
        return _moveprev(env, col);
    }
    else {
        fprintf(stderr, "task_move: _movetask\n");
        return _movetask(env, id, col);
    }
}

// roachme: maybe it's better to malloc curr?
char *task_getcurr(char *env)
{
    if (load_toggles(env) == FALSE)
        return NULL;
    else if (curr[0] == '\0')
        return NULL;
    return curr;
}

char *task_getprev(char *env)
{
    if (load_toggles(env) == FALSE)
        return NULL;
    else if (prev[0] == '\0')
        return NULL;
    return prev;
}

int task_swap(char *env)
{
    if (load_toggles(env) == FALSE) {
        fprintf(stderr, "task_swap: could not load toggles\n");
        return 1;
    }
    else if (curr[0] == '\0' || prev[0] == '\0') {
        fprintf(stderr, "task_swap: curr or prev toggles not found\n");
        return 1;
    }

    save_toggle(env, curr, MARKPREV);
    save_toggle(env, prev, MARKCURR);
    return 0;
}
