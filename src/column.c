/*
    Implement module column for tman project.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include <string.h>

#include "common.h"
#include "tman.h"
#include "column.h"


#ifdef MYTEST
#define TMANDB "/home/roach/trash/tman/.tman"
#define TMANTASK "/home/roach/trash/tman/tasks"
#endif

char envs[NENV][ENVSIZ];

char *formpath(char *dst, char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsprintf(dst, fmt, arg);
    va_end(arg);
    return dst;
}

#ifdef MYTEST
int elog(int status, char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    fprintf(stderr, "%s: ", "tman");
    vfprintf(stderr, fmt, arg);
    fprintf(stderr, "\n");
    va_end(arg);
    return status;
}
#endif


struct columns {
    int size;
    struct column col;
};

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

static int envsave()
{
    FILE *fp = fopen(tmanfs.fstate, "w");

    if (fp == NULL)
        return elog(1, "could not save env state\n");

    for (int i = 0; i < NENV; ++i) {
        fprintf(fp, "%s\n", envs[i]);
    }
    return fclose(fp);
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
    char line[BUFSIZ + 1];
    char idpath[BUFSIZ + 1];
    char envpath[BUFSIZ + 1];

    if ((dp = opendir(formpath(envpath, "%s/%s", tmanfs.task, env))) == NULL) {
        fprintf(stderr, "could not open env dir: %s\n", envpath);
        return 1;
    }

    memset(&taskids, 0, sizeof(taskids));
    for (taskids.idx = 0; (dir = readdir(dp)) != NULL && taskids.idx < NTASKS; ++taskids.idx) {
        if (dir->d_name[0] == '.' || dir->d_type != DT_DIR) {
            --taskids.idx; /* don't count this one */
            continue;
        }

        char *id = dir->d_name;
        if ((fp = fopen(formpath(idpath, "%s/%s/.tman/col", envpath, id), "r")) == NULL) {
            fprintf(stderr, "could not open col file: %s\n", idpath);
            continue;
        }

        fgets(line, BUFSIZ, fp);
        sscanf(line, "%*s : %10s\n", tag);

        taskids.ids[taskids.idx].col = column_setmark(tag);
        strcpy(taskids.ids[taskids.idx].id, dir->d_name);
        fclose(fp);
    }
    if (taskids.idx >= NTASKS)
        elog(1, "tasks limit in environment: %d", NTASKS);
    return closedir(dp);
}

int column_show(void)
{
    char *cenv = column_getcenv();
    int ncoltab = sizeof(coltab) / sizeof(coltab[0]);

    if (cenv[0] == '\0')
        return elog(1, "show:err: no curr env set");

    // FIXME: replace magic number with actual size of coltab
    for (int i = 0; i < 7; ++i)
        for (int k = 0; k < taskids.idx; ++k)
            if (strcmp(coltab[i].tag, taskids.ids[k].col.tag) == 0)
                printf("%c %s : %s\n", taskids.ids[k].col.mark, taskids.ids[k].id, taskids.ids[k].col.tag);
    printf("\n");
    return 0;
}

static int _save(char *env, char *id, char *tag)
{
    FILE *fp;
    char envpath[BUFSIZ];

    sprintf(envpath, "%s/%s/%s/.tman/col", tmanfs.task, env, id);
    if ((fp = fopen(envpath, "w")) == NULL)
        return elog(1, "could not save %s", envpath);

    fprintf(fp, "col : %s\n", tag);
    return fclose(fp);
}

static int save(void)
{
    char *cenv = column_getcenv();

    if (cenv[0] == '\0')
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
    char *cenv = column_getcenv();

    if (column_envinit() != 0)
        return elog(1, "err: column_envinit");
    if (cenv[0] != '\0')
        return load(cenv);
    return 0;
}

int column_envinit()
{
    FILE *fp;
    char line[BUFSIZ];
    char envpath[BUFSIZ];

    if ((fp = fopen(formpath(envpath, "%s/state", tmanfs.db), "r")) == NULL)
        elog(1, "could not open env state file %s\n", envpath);

    for (int i = 0; fgets(line, BUFSIZ, fp) != NULL && i < NENV; ++i)
        sscanf(line, "%s", envs[i]);
    return fclose(fp);
}

/* column_markid: Add a new task ID's col file */
int column_markid(char *id)
{
    FILE *fp;
    char *cenv = column_getcenv();
    char idpath[BUFSIZ];

    if (envs[CENV][0] == '\0')
        return elog(1, "column_markid: no current env set");

    sprintf(idpath, "%s/%s/%s/.tman/col", tmanfs.task, cenv, id);
    if ((fp = fopen(idpath, "w")) == NULL) {
        return elog(1, "could not create col file: %s\n", idpath);
    }

    strcpy(taskids.ids[taskids.idx].id, id);
    taskids.ids[taskids.idx].col = column_setmark(MARKDEF);
    ++taskids.idx;

    fprintf(fp, "col : %s\n", MARKDEF);
    return fclose(fp);
}

char *column_getcid()
{
    if (envs[CENV][0] == '\0') {
        fprintf(stderr, "current env not set\n");
        return NULL;
    }
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0)
            return taskids.ids[i].id;
    return NULL;
}

char *column_getpid()
{
    if (envs[CENV][0] == '\0') {
        fprintf(stderr, "current env not set\n");
        return NULL;
    }
    for (int i = 0; i < taskids.idx; ++i)
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0)
            return taskids.ids[i].id;
    return NULL;
}

int column_addcid(char *id)
{
    int idfound = FALSE;

    if (envs[CENV][0] == '\0') {
        fprintf(stderr, "current env not set\n");
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
            taskids.ids[i].isset = TRUE;
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
    /* Move previous task ID to deafult column */
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
    int cidfound = FALSE;
    int pidfound = FALSE;

    /* Find current and previous task IDs */
    for (int i = 0; i < taskids.idx; ++i) {
        if (strcmp(taskids.ids[i].col.tag, MARKCURR) == 0)
            cidfound = TRUE;
        if (strcmp(taskids.ids[i].col.tag, MARKPREV) == 0)
            pidfound = TRUE;
    }
    if (cidfound == FALSE || pidfound == FALSE)
        return elog(1, "current or previous task ID(s) not found");

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
    return envs[CENV];
}

char *column_getpenv()
{
    return envs[PENV];
}

int column_delcenv()
{
    strncpy(envs[CENV], envs[PENV], ENVSIZ);
    memset(envs[PENV], 0, sizeof(envs[PENV]));
    if (envs[CENV][0] != '\0')
        load(envs[CENV]);
    else
        memset(&taskids, 0, sizeof(taskids));
    return envsave();
}

int column_delpenv()
{
    memset(envs[PENV], 0, sizeof(envs[PENV]));
    return envsave();
}

int column_swapenv()
{
    char tmp[ENVSIZ];

    if (strlen(envs[CENV]) == 0 || strlen(envs[CENV]) == 0)
        return elog(1, "current or previous env not set");
    strncpy(tmp, envs[CENV], ENVSIZ);
    strncpy(envs[CENV], envs[PENV], ENVSIZ);
    strncpy(envs[PENV], tmp, ENVSIZ);
    /* Save new current env and load its task IDs */
    return !(envsave() == 0 && load(envs[CENV]) == 0);
}

int column_addcenv(char *env)
{
    strncpy(envs[PENV], envs[CENV], ENVSIZ);
    strncpy(envs[CENV], env, ENVSIZ);
    /* Save new current env and load its task IDs */
    return !(envsave() == 0 && load(env) == 0);
}

#ifdef MYTEST
int main(int arcg, char **argv)
{
    column_init();
    load("test");
    show();
    //column_delcid();
    column_swapid();
    //column_addcid("test1");
    //column_addcid("test2");
    show();
    return 0;
}
#endif
