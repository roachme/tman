/*
    Implement module column for tman project.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>

#include "tman.h"
#include "state.h"
#include "common.h"
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

static struct taskids taskids;      /* tasks per environment */

static struct column column_setmark(char *tag)
{
    int colsiz = sizeof(coltab) / sizeof(coltab[0]);

    for (int i = 1; i < colsiz; ++i)
        if (strcmp(tag, coltab[i].tag) == 0)
            return coltab[i];
    return coltab[0];
}

static int _save(char *env, char *id, char *tag)
{
    FILE *fp;
    char envpath[BUFSIZ];

    sprintf(envpath, "%s/%s/%s/.tman/col", tmanfs.base, env, id);
    if ((fp = fopen(envpath, "w")) == NULL)
        return elog(1, "could not save %s", envpath);

    fprintf(fp, "col : %s\n", tag);
    return fclose(fp);
}

static int save(void)
{
    char *cenv = state_getcenv();

    if (cenv[0] == '\0')
        return elog(1, "save: no current env set");

    for (int i = 0; i < taskids.idx; ++i)
        if (taskids.ids[i].isset) {
            taskids.ids[i].isset = FALSE;
            _save(cenv, taskids.ids[i].id, taskids.ids[i].col.tag);
        }
    return 0;
}

static int _setcol(struct taskid *taskid, char *tag)
{
    taskid->isset = TRUE;
    taskid->col = column_setmark(tag);
    return 0;
}

static int _setcolid(char *id, char *tag)
{
    return 0;
}

static struct taskid _getcol(struct taskids *taskids, int idx)
{
    return taskids->ids[idx];
}

static int _tagext(char *tag)
{
    // TOTO: replace magic number with correct size
    for (int i = 0; i < 8; ++i)
        if (strcmp(coltab[i].tag, tag) == 0)
            return TRUE;
    return FALSE;
}

/* column_markid: will be needed by tman `list' command */
struct column column_getmark(char *id)
{
    int colsiz = sizeof(coltab) / sizeof(coltab[0]);

    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(id, taskids.ids[i].id) == 0)
            return taskids.ids[i].col;
    return coltab[0];
}

static int _reset()
{
    memset(&taskids, 0, sizeof(taskids));
    return 0;
}

static int load(char *env)
{
    int i;
    DIR *dp;
    FILE *fp;
    char tag[TAGSIZ + 1];
    struct dirent *dir;
    char line[BUFSIZ + 1];
    char idpath[BUFSIZ + 1];
    char envpath[BUFSIZ + 1];

    if (_reset())
        return elog(1, "could not reset taskids");
    else if (formpath(envpath, "%s/%s", tmanfs.base, env) == NULL)
        return elog(1, "%s: could not form path for task env", env);
    else if ((dp = opendir(envpath)) == NULL)
        return elog(1, "could not open env dir: %s\n", envpath);

    for (i = 0; (dir = readdir(dp)) != NULL && taskids.idx < NTASKS; ++i) {
        if (dir->d_name[0] == '.' || dir->d_type != DT_DIR) {
            --i; /* don't count this one */
            continue;
        } else if (formpath(idpath, "%s/%s/.tman/col", envpath, dir->d_name) == NULL) {
            elog(1, "%s: could not form path for task id", dir->d_name);
            continue;
        } else if ((fp = fopen(idpath, "r")) == NULL) {
            elog(1, "could not open col file: %s\n", idpath);
            continue;
        }

        fscanf(fp, "%*s : %4s\n", tag);
        taskids.ids[i].col = column_setmark(tag);
        strcpy(taskids.ids[i].id, dir->d_name);

        // Load env/task state
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0)
            state_setcid(taskids.ids[i].id);
        else if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0)
            state_setpid(taskids.ids[i].id);

        fclose(fp);
    }
    if (i >= NTASKS)
        elog(1, "tasks limit in environment: %d", NTASKS);
    taskids.idx = i;
    return closedir(dp);
}

int column_init(char *env)
{
    if (state_init(tmanfs.fstate, tmanfs.db))
        return 1;

    env = env != NULL ? env : state_getcenv();
    if (env[0] != '\0')
        return load(env);
    return 0;
}

/* column_markid: Add a new task ID's col file */
int column_markid(char *id)
{
    FILE *fp;
    char *cenv = state_getcenv();
    char idpath[BUFSIZ];

    if (state_getcenv() == NULL)
        return elog(1, "column_markid: no current env set");

    sprintf(idpath, FMTCOL, tmanfs.base, cenv, id);
    if ((fp = fopen(idpath, "w")) == NULL)
        return elog(1, "could not create col file: %s\n", idpath);

    // FIXME: add a bound checker.
    strcpy(taskids.ids[taskids.idx].id, id);
    taskids.ids[taskids.idx].col = column_setmark(MARKDEF);
    ++taskids.idx;

    fprintf(fp, "col : %s\n", MARKDEF);
    return fclose(fp);
}

int column_addcid(char *id)
{
    int idfound = FALSE;

    if (state_getcenv() == NULL)
        return elog(1, "current env not set");

    for (int i = 0; i < taskids.idx; ++i) {
        if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0) {
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKDEF);
        }
        else if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0) {
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKPREV);
        }
        else if (strcmp(taskids.ids[i].id, id) == 0) {
            idfound = TRUE;
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKCURR);
        }
    }

    if (idfound)
        return !(state_addcid(id) == 0 && save() == 0);
    return elog(1, "no such task ID: %s", id);
}

int column_delcid(void)
{
    int cidfound = FALSE;

    for (int i = 0; i < taskids.idx; ++i) {
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0) {
            cidfound = TRUE;
            taskids.ids[i].isset = FALSE;
            taskids.ids[i].col = column_setmark(MARKDEF);
        }
        else if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0) {
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKCURR);
        }
    }

    if (cidfound == TRUE)
        return !(state_delcid() == 0 && save() == 0);
    return elog(1, "no current ID is set to delete: %d", cidfound);
}

int column_delpid()
{
    /* Move previous task ID to deafult column */
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0) {
            taskids.ids[i].isset = TRUE;
            taskids.ids[i].col = column_setmark(MARKDEF);
            return !(state_delpid() == 0 && save() == 0);
        }

    return elog(1, "no previous ID is set to delete");
}

int column_swapid()
{
    int cidfound = FALSE;
    int pidfound = FALSE;

    for (int i = 0; i < taskids.idx; ++i) {
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0) {
            cidfound = TRUE;
            _setcol(&taskids.ids[i], MARKPREV);
        }
        else if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0) {
            pidfound = TRUE;
            _setcol(&taskids.ids[i], MARKCURR);
        }
    }
    if (cidfound == TRUE && pidfound == TRUE)
        return !(state_swapids() == 0 && save() == 0);
    return 1;
}

/*
 * @return 0 - current or previous deleted. or no need to delete.
 * @return 1 - if could not delete special task ID
*/
int column_delspec(char *id)
{
    char *cid = state_getcid();
    char *pid = state_getpid();

    // TODO: Gotta check that `id' is not NULL.
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
    if(strcmp(tag, MARKCURR) == 0 || strcmp(tag, MARKPREV) == 0)
        return elog(1, "column_moveid: cant move to curr or prev column");

    if (_tagext(tag) == FALSE)
        return elog(1, "%s: no such column in env", tag);

    // TODO: should we change directory if current task ID is moved?
    for (int i = 0; i < taskids.idx; ++i) {
        if (strcmp(taskids.ids[i].id, id) == 0) {
            if (strcmp(taskids.ids[i].col.tag, tag) == 0)
                return 0;
            else if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0)
                column_delcid();
            else if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0)
                column_delpid();
            return _setcol(&taskids.ids[i], tag);
        }
    }
    return elog(1, "could not find id : %s", id);
}

int column_swapenvs()
{
    return !(state_swapenvs() == 0 && load(state_getcenv()) == 0);
}

int column_addcenv(char *env)
{
    return !(state_addcenv(env) == 0 && load(env) == 0);
}
