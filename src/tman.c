#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "column.h"
#include "dir.h"
#include "tman.h"
#include "env.h"
#include "task.h"
#include "unit.h"
#include "hook.h"
#include "unit.h"
#include "common.h"
#include "osdep.h"
#include "errmod.h"
#include "config.h"

// TODO: Make NOT global.
struct tmanstruct tmanfs;

static char *taskenv, *taskid;

static char task_currid[IDSIZ + 1], task_previd[IDSIZ + 1];
static char task_currenv[ENVSIZ + 1], task_prevenv[ENVSIZ + 1];

static tman_ctx_t *mkctx()
{
    tman_ctx_t *ctx;

    if ((ctx = malloc(sizeof(tman_ctx_t))) == NULL)
        return NULL;

    memset(ctx, 0, sizeof(tman_ctx_t));
    return ctx;
}

static int mkfs_vars()
{
    // TODO: Add a variable for task db as well.
    sprintf(tmanfs.base,   "%s",    config.base);
    sprintf(tmanfs.db,     "%s/%s", tmanfs.base, ".tman");
    sprintf(tmanfs.finit,  "%s/%s", tmanfs.db,   "inited");
    sprintf(tmanfs.fstate, "%s/%s", tmanfs.db,   "state");
    sprintf(tmanfs.pgn,    "%s/%s", tmanfs.base, ".pgn");
    sprintf(tmanfs.pgnins, "%s",    config.pgnins);
    return TMAN_OK;
}

int tman_mkfs(void)
{
    //int status;

    if (MKDIR(tmanfs.base))
        return elog(1, "could not create directory %s", tmanfs.base);
    else if (MKDIR(tmanfs.base))
        return elog(1, "could not create directory %s", tmanfs.base);
    else if (MKDIR(tmanfs.pgn))
        return elog(1, "could not create directory %s", tmanfs.pgn);
    else if (MKDIR(tmanfs.pgnins))
        return elog(1, "could not create directory %s", tmanfs.pgnins);
    else if (MKDIR(tmanfs.db))
        return elog(1, "could not create directory %s", tmanfs.db);
    else if (TOUCH(tmanfs.fstate))
        return elog(1, "could not create file %s", tmanfs.fstate);
    else if (TOUCH(tmanfs.finit))
        return elog(1, "could not create init file %s", tmanfs.finit);
    return TMAN_OK;
}

struct tman_context *tman_init(void)
{
    if (config_init()) {
        emod_set(TMAN_ECONF);
        return NULL;
    }
    else if (mkfs_vars()) {
        emod_set(TMAN_ESYSVAR);
        return NULL;
    }
    return mkctx();
}

int tman_setup(int setuplvl)
{
    int status;

    status = TMAN_OK;
    if (setuplvl == TMAN_SETUPSOFT)
        return TMAN_OK;
    else if (setuplvl == TMAN_SETUPHARD) {
        elog(1, "TMAN_SETUPHARD");
        if ((status = tman_mkfs()) != TMAN_OK) {
            elog(1, "in tmat_setup");
            status = emod_set(TMAN_EINIT);
        }
    }
    else if (setuplvl == TMAN_SETUPCHECK) {
        //elog(1, "TMAN_SETUPCHECK '%s'", tmanfs.fstate);
        if (ISFILE(tmanfs.finit) != TRUE) {
            status = emod_set(TMAN_EINIT);
        }
        else if (env_init(tmanfs.fstate) != 0)
            return elog(1, "err: column_envinit");
        // roach: is it a good idea to init module column in general
        // and here?
    }
    return status;
}

int tman_pwd()
{
    char *cid, *cenv;

    if ((cenv = env_getcurr()) == NULL) {
        elog(1, "internal error: no current env set");
        return 1;
    }
    cid = task_getcurr(cenv);
    printf("PWD: %s/%s/%s\n", tmanfs.base, cenv, cid ? cid : "");
    return TMAN_OK;
}

int tman_id_add(tman_ctx_t *ctx, char *env, char *id, struct tman_id_add_opt *opt)
{
    // TODO: Add support to pass unit values into unit_add()
    taskenv = env;
    taskid  = id;
    // TODO: maybe it's better to move units to ctx?
    struct unitbin units[NKEYS] = {0};
    char *cenv = env_getcurr();
    char *col = MARKCURR;

    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    else if (task_exists(taskenv, taskid) == TRUE)
        return emod_set(TMAN_EIDEXISTS);
    else if (_chkid(taskid) == FALSE)
        return emod_set(TMAN_EILLEGID);
    else if (column_exists(col) == FALSE)
        return emod_set(TMAN_ECOLNEXIST);
    else if (unit_check(units) == FALSE)
        return emod_set(TMAN_EILLEGUNIT);

    if (imkdir(tmanfs.base, taskenv, taskid) != 0)
        return emod_set(TMAN_ETASKMKDIR);
    else if (unit_addbin(taskenv, taskid, units) != 0)
        return emod_set(TMAN_ETASKMKUNIT);
    // TODO: add support to not mark task as current
    // depending on option.
    else if (task_add(taskenv, taskid, col))
        return emod_set(TMAN_EADDID);
    else if (hookact("add", taskenv, taskid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_del(tman_ctx_t *ctx, char *env, char *id, struct tman_id_del_opt *opt)
{
    // FIXME: causes error when delete current task in previous env
    taskenv = env;
    taskid  = id;

    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    else if (taskid == NULL && (taskid = task_getcurr(taskenv)) == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (task_exists(taskenv, taskid) == FALSE)
        return emod_set(TMAN_ENOSUCHID);

    if (hookact("del", taskenv, taskid))
        return emod_set(TMAN_EHOOK);
    else if (unit_delbin(taskenv, taskid))
        return emod_set(TMAN_ETASKRMUNIT);
    // TODO: gotta change the order call: task_del goes before irmdir
    else if (irmdir(tmanfs.base, taskenv, taskid))
        return emod_set(TMAN_ETASKRMDIR);
    else if (task_del(taskenv, taskid))
        return emod_set(TMAN_EDELID);
    return TMAN_OK;
}

int tman_id_prev(tman_ctx_t *ctx, struct tman_id_prev_opt *opt)
{
    char *cenv;

    if ((cenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (task_getcurr(cenv) == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (task_getprev(cenv) == NULL)
        return emod_set(TMAN_ENOPREVID);
    else if (task_swap(cenv))
        return emod_set(TMAN_ESWAPIDS);
    else if (hookact("prev", env_getcurr(), task_getcurr(cenv)))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_sync(tman_ctx_t *ctx, struct tman_id_sync_opt *opt)
{
    char *cid, *cenv;

    if ((cenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if ((cid = task_getcurr(cenv)) == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (hookact("sync", cenv, cid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_set (tman_ctx_t *ctx, char *env, char *id, struct unitbin *unitbin, struct tman_id_set_opt *opt)
{
    taskid  = id;
    taskenv = env;

    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    else if (taskid == NULL && (taskid = task_getcurr(taskenv)) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (task_exists(taskenv, taskid) == FALSE)
        return emod_set(TMAN_ENOSUCHID);
    else if (unit_setbin(taskenv, taskid, unitbin)) {
        elog(1, "%s: could not set unit values", taskid);
        return emod_set(TMAN_NODEF_ERR);
    }

    // TODO: change task directory if id unit was changed
    // TODO: update task id status as well.
    return TMAN_OK;
}

// TODO: add support to switch to task in another environment.
int tman_id_use(tman_ctx_t *ctx, char *env, char *id, struct tman_id_use_opt *opt)
{
    taskid = id;
    taskenv = env != NULL ? env : env_getcurr();

    if (taskenv == NULL)
        return emod_set(TMAN_ENOCURRENV);
    if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    else if (_chkenv(taskenv) == FALSE)
        return emod_set(TMAN_EILLEGENV);
    else if (taskid == NULL)
        return emod_set(TMAN_EREQRID);
    else if (task_exists(taskenv, taskid) == FALSE)
        return emod_set(TMAN_EMISSID);

    /* switch to new current environment.  */
    if (taskenv != env_getcurr() && env_addcenv(taskenv) != 0)
        return emod_set(TMAN_ESWITCHENV);
    return task_move(taskenv, taskid, MARKCURR);
}

int tman_id_move(tman_ctx_t *ctx, char *id, char *dst, char *src)
{
    /*
    char dstid[BUFSIZ + 1] = {0};

    taskid = id;
    taskenv = src ? src : column_getcenv();
    sprintf(dstid, "%s/%s/%s", tmanfs.base, dst, id);

    if (env_exists(dst) == FALSE) {
        elog(1, "no such destination env");
        return emod_set(TMAN_NODEF_ERR);
    }
    else if (taskenv == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_exists(taskenv) == FALSE) {
        elog(1, "no such source env");
        return emod_set(TMAN_NODEF_ERR);
    }
    else if (task_exists(taskenv, taskid) == FALSE)
        emod_set(TMAN_ENOSUCHID);
    else if (task_exists(dst, taskid)) {
        elog(1, "cannot move '%s': task id exists in env '%s'", taskid, dst);
        return emod_set(TMAN_NODEF_ERR);
    }

    if (imove(tmanfs.base, taskid, dst, taskenv)) {
        elog(1, "%s: could not move task to %s", taskid, dst);
        return emod_set(TMAN_NODEF_ERR);
    }
    else if (strcmp(column_getcid(), taskid) == 0)
        return column_delcid();
    else if (strcmp(column_getpid(), taskid) == 0)
        return !column_swapid() && !column_delcid();
    return TMAN_OK;
    */
    return 0;
}

/*
 @param env char * | NULL (then list the current env)
 @return struct item * | NULL (if error happened)
*/
int tman_id_list(tman_ctx_t *ctx, char *env, struct tman_id_list_opt *opt)
{
    DIR *ids;
    size_t i = 0;
    struct dirent *ent;
    struct unitbin bunit[NKEYS];
    char pathname[PATHSIZ + 1];
    taskenv = env;

    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);

    sprintf(pathname, "%s/%s", tmanfs.base, taskenv);
    if ((ids = opendir(pathname)) == NULL) {
        elog(1, "%s: could not read environment", taskenv);
        return emod_set(TMAN_NODEF_ERR);
    }

    while ((ent = readdir(ids)) != NULL && i < LSIZE) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        if (unit_getbin(bunit, taskenv, ent->d_name) == NULL) {
            fprintf(stderr, "tman_id_list %s: failde to get units\n", ent->d_name);
            continue;
        }

        /* TODO: remove check and warning because there might
         * case when no hooks executed or defined and it's ok.  */
        if (!hookls(ctx->list.ilist[i].pgn, taskenv, ent->d_name)) {
            fprintf(stderr, "tman_id_list %s: failed to get hookls output\n", ent->d_name);
        }
        strcpy(ctx->list.ilist[i].id, ent->d_name);
        strcpy(ctx->list.ilist[i].desc, bunit[4].val);
        ctx->list.ilist[i].col = column_getmark(taskenv, ctx->list.ilist[i].id);
        ++i;
    }
    ctx->list.num = i;
    closedir(ids);
    return TMAN_OK;
}

int tman_id_col(tman_ctx_t *ctx, char *env, char *id, char *tag, struct tman_id_col_opt *opt)
{
    char *cenv = env_getcurr();
    taskenv = env ? env : cenv;
    taskid = id ? id : task_getcurr(cenv);

    // TODO: does not change CWD if curr task was moved.
    // The problem in CLI command `col'.
    if (taskenv == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (taskid == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (task_exists(taskenv, taskid) == FALSE)
        return emod_set(TMAN_ENOSUCHID);
    return task_move(taskenv, taskid, tag);
}

int tman_id_cat(tman_ctx_t *ctx, char *env, char *id, struct tman_id_cat_opt *opt)
{
    int status;
    taskid = id;
    taskenv = env;

    status = TMAN_OK;
    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    else if (taskid == NULL && (taskid = task_getcurr(taskenv)) == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (task_exists(taskenv, taskid) == FALSE)
        return emod_set(TMAN_ENOSUCHID);

    // TODO: don't wanna lose plugin units if builtin ones
    // failed to parse. But gotta make user return value
    // make any sense for caller.

    /* No need to check return value because there might case
     * that no hooks are defined or executed */
    ctx->units.pgn = hookcat(ctx->units.pgn, taskenv, taskid);
    if (unit_getbin(ctx->units.bin, taskenv, taskid) == NULL) {
        status = emod_set(TMAN_NODEF_ERR);
        elog(1, "tman_id_cat: failed to get builtn units");
    }
    return status;
}

char *tman_id_getcurr(tman_ctx_t *ctx, char *env)
{
    taskenv = env;

    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL) {
        emod_set(TMAN_ENOCURRENV);
        return NULL;
    }
    else if ((taskid = task_getcurr(taskenv)) == NULL)
        return NULL;
    return strncpy(task_currid, taskid, IDSIZ);
}

char *tman_id_getprev(tman_ctx_t *ctx, char *env)
{
    taskenv = env;

    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL) {
        emod_set(TMAN_ENOCURRENV);
        return NULL;
    }
    else if ((taskid = task_getprev(taskenv)) == NULL)
        return NULL;
    return strncpy(task_previd, taskid, IDSIZ);
}

int tman_env_add(tman_ctx_t *ctx, char *env, struct tman_env_add_opt *opt)
{
    taskenv = env;

    if (taskenv == NULL)
        return emod_set(TMAN_EREQRENV);
    else if (env_exists(taskenv))
        return emod_set(TMAN_EENVEXISTS);
    else if (_chkenv(taskenv) == FALSE)
        return emod_set(TMAN_EILLEGENV);
    else if (emkdir(tmanfs.base, taskenv)) {
        elog(1, "%s: could not create env directory", taskenv);
        return emod_set(TMAN_NODEF_ERR);
    }
    return env_addcenv(env);
}

int tman_env_del(tman_ctx_t *ctx, char *env, struct tman_env_del_opt *opt)
{
    taskenv = env;

    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    else if (ermdir(tmanfs.base, taskenv)) {
        elog(1, "%s: could not delete env directory", taskenv);
        return emod_set(TMAN_NODEF_ERR);
    }
    return env_delcenv();
}

int tman_env_list(tman_ctx_t *ctx, struct tman_env_list_opt *opt)
{
    return TMAN_OK;
}

int tman_env_prev(tman_ctx_t *ctx, struct tman_env_prev_opt *opt)
{
    if (env_getcurr() == NULL)
        return emod_set(TMAN_ENOCURRENV);
    if (env_getprev() == NULL)
        return emod_set(TMAN_ENOPREVENV);
    if (env_swapenvs())
        return emod_set(TMAN_ENOPREVENV);
    return TMAN_OK;
}

int tman_env_set(tman_ctx_t *ctx, char *env, struct tman_env_set_opt *opt)
{
    return TMAN_OK;
}

int tman_env_use(tman_ctx_t *ctx, char *env, struct tman_env_use_opt *opt)
{
    taskenv = env;

    if (taskenv == NULL)
        return emod_set(TMAN_EREQRENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    return env_addcenv(taskenv);
}

char *tman_env_getcurr(tman_ctx_t *ctx)
{
    return env_getcurr();
}

char *tman_env_getprev(tman_ctx_t *ctx)
{
    return env_getprev();
}


int tman_isplugin(const char *pgn)
{
    return isplugin(pgn);
}

int tman_plugin(tman_ctx_t *ctx, int argc, char **argv, struct tman_pgn_opt *opt)
{
    return plugin(argc, argv);
}

const char *tman_strerror(void)
{
    return emod_strerror();
}

int tman_deinit(struct tman_context *ctx)
{
    unit_delpgn(ctx->units.pgn);
    free(ctx);
    return TMAN_OK;
}
