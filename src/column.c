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
#include "tman.h"
#include "column.h"

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

struct taskids taskids;      /* tasks per environment */
static char colpath[BUFSIZ + 1];

static char *genpath(char *env, char *id)
{
    sprintf(colpath, "%s/%s/%s/.tman/col", tmanfs.base, env, id);
    return colpath;
}

int column_exists(char *col)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(coltab); ++i)
        if (strncmp(col, coltab[i].tag, COLSIZ) == 0)
            return TRUE;
    return FALSE;
}

/* column_markid: will be needed by tman `list' command */
struct column column_getmark(char *env, char *id)
{
    int i;
    FILE *fp;
    char tag[TAGSIZ + 1];

    // NOTE: hotfix
    if ((fp = fopen(genpath(env, id), "r")) == NULL)
        return coltab[7];

    fscanf(fp, "%*s : %4s\n", tag);
    fclose(fp);

    for (i = 0; i < ARRAY_SIZE(coltab); ++i) {
        if (strncmp(tag, coltab[i].tag, TAGSIZ) == 0)
            return coltab[i];
    }
    return coltab[i];
}

struct column column_setmark(char *tag)
{
    int colsiz = sizeof(coltab) / sizeof(coltab[0]);

    for (int i = 1; i < colsiz; ++i)
        if (strcmp(tag, coltab[i].tag) == 0)
            return coltab[i];
    return coltab[0];
}

static int load(char *env)
{
    FILE *fp;
    DIR *dp;
    char tag[TAGSIZ + 1];
    struct dirent *dir;
    char idpath[BUFSIZ + 1];
    char envpath[BUFSIZ + 1];

    if ((dp = opendir(formpath(envpath, "%s/%s", tmanfs.base, env))) == NULL)
        return elog(1, "could not open env dir: %s", envpath);

    memset(&taskids, 0, sizeof(taskids));
    for (taskids.idx = 0; (dir = readdir(dp)) != NULL && taskids.idx < NTASKS; ++taskids.idx) {
        if (dir->d_name[0] == '.' || dir->d_type != DT_DIR) {
            --taskids.idx; /* don't count this one */
            continue;
        }

        char *id = dir->d_name;
        if ((fp = fopen(formpath(idpath, "%s/%s/.tman/col", envpath, id), "r")) == NULL) {
            elog(1, "could not open col file: %s", idpath);
            continue;
        }

        fscanf(fp, "%*s : %4s\n", tag);
        taskids.ids[taskids.idx].col = column_setmark(tag);
        strcpy(taskids.ids[taskids.idx].id, dir->d_name);
        fclose(fp);
    }
    if (taskids.idx >= NTASKS)
        elog(1, "tasks limit in environment: %d", NTASKS);
    return closedir(dp);
}

static int _save(char *env, char *id, char *tag)
{
    FILE *fp;

    if ((fp = fopen(genpath(env, id), "w")) == NULL)
        return elog(1, "could not save %s", colpath);

    fprintf(fp, "col : %s\n", tag);
    return fclose(fp);
}

static int save(void)
{
    char *cenv = column_getcenv();

    if (cenv == NULL)
        return elog(1, "save: no current env set");

    for (int i = 0; i < taskids.idx; ++i)
        if (taskids.ids[i].isset) {
            taskids.ids[i].isset = FALSE;
            _save(cenv, taskids.ids[i].id, taskids.ids[i].col.tag);
        }
    return 0;
}

int column_init()
{
    char *cenv;

    if (env_init(tmanfs.fstate) != 0)
        return elog(1, "err: column_envinit");
    if ((cenv = env_getcurr()) != NULL)
        return load(cenv);
    return 0;
}

/* column_markid: Add a new task ID's col file */
int column_markid(char *id)
{
    FILE *fp;
    char *cenv = column_getcenv();

    if (env_getcurr() == NULL)
        return elog(1, "column_markid: no current env set");
    else if ((fp = fopen(genpath(cenv, id), "w")) == NULL)
        return elog(1, "could not create col file: %s", colpath);

    strcpy(taskids.ids[taskids.idx].id, id);
    taskids.ids[taskids.idx].col = column_setmark(MARKDEF);
    ++taskids.idx;

    fprintf(fp, "col : %s\n", MARKDEF);
    return fclose(fp);
}

char *column_getcid()
{
    if (env_getcurr() == NULL) {
        elog(1, "current env not set");
        return NULL;
    }
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0)
            return taskids.ids[i].id;
    return NULL;
}

char *column_getpid()
{
    if (env_getcurr() == NULL) {
        elog(1, "current env not set");
        return NULL;
    }
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0)
            return taskids.ids[i].id;
    return NULL;
}

int column_addcid(char *id)
{
    int idfound = FALSE;

    if (env_getcurr() == NULL) {
        elog(1, "current env not set");
        return 1;
    }

    /* Move previous task ID to default column, i.e. blog */
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0) {
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKDEF);
        }

    /* Move old current task ID to previous column */
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0) {
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKPREV);
        }

    /* Move new task ID to current column */
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].id, id) == 0) {
            idfound = TRUE;
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKCURR);
        }

    if (idfound)
        return save();
    return elog(1, "no such task ID: %s", id);
}

int column_delcid(void)
{
    int cidfound = FALSE;

    /* Move current task ID to deafult column */
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0) {
            cidfound = TRUE;
            taskids.ids[i].isset = FALSE;
            taskids.ids[i].col = column_setmark(MARKDEF);
        }

    /* Move previous task ID to current column */
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0) {
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKCURR);
        }

    if (cidfound == TRUE)
        return save();
    return elog(1, "no current ID is set to delete: %d", cidfound);
}

int column_delpid()
{
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0) {
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKDEF);
            return save();
        }
    return elog(1, "no previous ID is set to delete");
}

int column_swapid()
{
    if (column_getcid() == NULL || column_getpid() == NULL)
        return 1;

    /* Swap current and previous task IDs */
    for (int i = 0; i < taskids.idx; ++i) {
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0) {
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKPREV);
        } else if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0) {
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKCURR);
        }
    }
    return save();
}

/*
 * @return 0 - current or previous deleted. or no need to delete.
 * @return 1 - if could not delete special task ID
*/
int column_delspec(char *id)
{
    char *cid = column_getcid();
    char *pid = column_getpid();

    if (strcmp(cid, id) == 0) {
        if (column_delcid() != 0)
            return elog(1, "could not delete current task id");
    }
    else if (strcmp(pid, id) == 0) {
        if (column_delpid() != 0)
            return elog(1, "could not delete previous task id");
    }
    return 0;
}

int column_moveid(char *id, char *tag)
{
    int tagfound = FALSE;

    if(strcmp(tag, MARKCURR) == 0 || strcmp(tag, MARKPREV) == 0)
        return elog(1, "column_moveid: cant move to curr or prev column");

    // TODO: check that it's the same tag,
    // so no need to rewrite to file.

    for (int i = 0; i < 8; ++i)
        if (strcmp(coltab[i].tag, tag) == 0)
            tagfound = TRUE;

    if (tagfound == FALSE)
        return elog(1, "%s: no such column in env", tag);

    // TODO: should we change directory if current task ID is moved?
    for (int i = 0; i < taskids.idx; ++i) {
        if (strcmp(taskids.ids[i].id, id) == 0) {
            if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0) {
                /* Update curr & prev task IDs and then move to column */
                column_delcid();
                taskids.ids[i].isset = TRUE;
                taskids.ids[i].col = column_setmark(tag);
            }
            else if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0) {
                /* Update prev task IDs and then move to column */
                column_delpid();
                taskids.ids[i].isset = TRUE;
                taskids.ids[i].col = column_setmark(tag);
            }
            else {
                taskids.ids[i].isset = TRUE;
                taskids.ids[i].col = column_setmark(tag);
            }
            return save();
        }
    }
    return elog(1, "could not find id : %s", id);
}

char *column_getcenv()
{
    return env_getcurr();
}

char *column_getpenv()
{
    return env_getprev();
}

int column_delcenv()
{
    char *cenv;

    if (env_delcenv())
        return 1;
    if ((cenv = env_getcurr()) != NULL)
        return load(cenv);
    return 0;
}

int column_delpenv()
{
    char *cenv;

    if (env_delpenv())
        return 1;
    if ((cenv = env_getcurr()) != NULL)
        return load(cenv);
    return 0;
}

int column_swapenv()
{
    return !(env_swapenvs() == 0 && load(env_getcurr()) == 0);
}

int column_addcenv(char *env)
{
    return !(env_addcenv(env) == 0 && load(env) == 0);
}
