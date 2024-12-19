#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "dir.h"
#include "tman.h"
#include "unit.h"
#include "hook.h"
#include "unit.h"
#include "common.h"
#include "column.h"
#include "osdep.h"
#include "errmod.h"
#include "config.h"

// TODO: Make NOT global.
struct tmanstruct tmanfs;

static char *taskenv, *taskid;

static tman_ctx_t *mkctx()
{
    tman_ctx_t *ctx;

    if ((ctx = malloc(sizeof(tman_ctx_t))) == NULL)
        return NULL;

    memset(ctx, 0, sizeof(tman_ctx_t));
    return ctx;
}

static BOOL env_exists(char *env)
{
    char pathname[PATHSIZ + 1];
    sprintf(pathname, "%s/%s", tmanfs.base, env);
    return ISDIR(pathname);
}

static BOOL id_exists(char *env, char *id)
{
    char pathname[PATHSIZ + 1];
    sprintf(pathname, "%s/%s/%s", tmanfs.base, env, id);
    return ISDIR(pathname);
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
        // roach: is it a good idea to init module column in general
        // and here?
        else if (column_init()) {
            elog(1, "column_init: error: could not init");
            status = emod_set(TMAN_EINIT);
        }
    }
    return status;
}

int tman_pwd()
{
    char *cid = column_getcid();
    printf("PWD: %s/%s/%s\n", tmanfs.base, column_getcenv(), cid ? cid : "");
    return TMAN_OK;
}

int tman_id_add(tman_ctx_t *ctx, char *env, char *id, struct tman_id_add_opt *opt)
{
    // TODO: Add support to pass unit values into unit_add()
    taskid  = id;
    // TODO: maybe it's better to move units to ctx?
    struct unitbin units[NKEYS] = {0};
    char *cenv = column_getcenv();
    taskenv = env != NULL ? env : cenv;

    if (taskenv == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    else if (id_exists(taskenv, taskid) == TRUE)
        return emod_set(TMAN_EIDEXISTS);
    else if (_chkid(taskid) == FALSE)
        return emod_set(TMAN_EILLEGID);

    if (imkdir(tmanfs.base, taskenv, taskid) != 0)
        return emod_set(TMAN_ETASKMKDIR);
    else if (unit_addbin(taskenv, taskid, units) != 0)
        return emod_set(TMAN_ETASKMKUNIT);
    else if (hookact("add", taskenv, taskid))
        return emod_set(TMAN_EHOOK);
    else if (column_markid(taskid)) {
        elog(1, "column_mark: failed");
        return emod_set(TMAN_NODEF_ERR);
    }
    if (opt->noswitch == FALSE && strcmp(taskenv, cenv) == 0 && column_addcid(id) != 0) {
        elog(1, "column_addcid: failed");
        return emod_set(TMAN_NODEF_ERR);
    }
    return TMAN_OK;
}

int tman_id_del(tman_ctx_t *ctx, char *env, char *id, struct tman_id_del_opt *opt)
{
    // FIXME: causes error when delete current task in previous env
    char *cid = column_getcid();
    char *pid = column_getpid();
    taskid  = id != NULL ? id : cid;
    taskenv = env != NULL ? env : column_getcenv();

    if (taskenv == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (!env_exists(taskenv))
        return emod_set(TMAN_ENOSUCHENV);
    else if (taskid == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (!id_exists(taskenv, taskid))
        return emod_set(TMAN_ENOSUCHID);

    if (hookact("del", taskenv, taskid))
        return emod_set(TMAN_EHOOK);
    else if (unit_delbin(taskenv, taskid))
        return emod_set(TMAN_ETASKRMUNIT);
    else if (irmdir(tmanfs.base, taskenv, taskid))
        return emod_set(TMAN_ETASKRMDIR);
    else if (column_delspec(taskid)) {
        elog(1, "%s: could not update special task IDs", taskid);
        return emod_set(TMAN_NODEF_ERR);
    }
    return TMAN_OK;
}

int tman_id_prev(tman_ctx_t *ctx, struct tman_id_prev_opt *opt)
{
    if (column_swapid())
        return emod_set(TMAN_ENOPREVID);
    if (hookact("prev", column_getcenv(), column_getcid()))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_sync(tman_ctx_t *ctx, struct tman_id_sync_opt *opt)
{
    char *cid  = column_getcid();
    char *cenv = column_getcenv();

    if (cid == NULL)
        return emod_set(TMAN_ENOCURRID);
    if (hookact("sync", cenv, cid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_set (tman_ctx_t *ctx, char *env, char *id, struct unitbin *unitbin, struct tman_id_set_opt *opt)
{
    taskid  = id != NULL ? id : column_getcid();
    taskenv = env != NULL ? env : column_getcenv();

    if (taskenv == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (!env_exists(taskenv))
        return emod_set(TMAN_ENOSUCHENV);
    else if (taskid == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (!id_exists(taskenv, taskid))
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
    taskenv = env != NULL ? env : column_getcenv();

    if (taskenv == NULL)
        return emod_set(TMAN_ENOCURRENV);
    if (!env_exists(taskenv))
        return emod_set(TMAN_ENOSUCHENV);
    else if (_chkenv(taskenv) == FALSE)
        return emod_set(TMAN_EILLEGENV);
    else if (taskid == NULL)
        return emod_set(TMAN_EREQRID);
    else if (id_exists(taskenv, taskid) == FALSE)
        return emod_set(TMAN_EMISSID);
    else if (taskenv != column_getcenv()) {
        fprintf(stderr, "trynna switch to task in another env\n");
        fprintf(stderr, "under development\n");
        return emod_set(TMAN_NODEF_ERR);
    }

    // TODO: it can't switch to task in non-current env.
    // Cuz it gotta switch env first.
    return column_addcid(taskid);
}

int tman_id_move(tman_ctx_t *ctx, char *id, char *dst, char *src)
{
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
    else if (!id_exists(taskenv, taskid))
        emod_set(TMAN_ENOSUCHID);
    else if (id_exists(dst, taskid)) {
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
    taskenv = env ? env : column_getcenv();

    if (taskenv == NULL)
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
        ctx->list.ilist[i].col = column_getmark(ctx->list.ilist[i].id);
        ++i;
    }
    ctx->list.num = i;
    closedir(ids);
    return TMAN_OK;
}

int tman_id_col(tman_ctx_t *ctx, char *env, char *id, char *tag, struct tman_id_col_opt *opt)
{
    taskid = id ? id : column_getcid();
    taskenv = env ? env : column_getcenv();

    if (taskenv == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (taskid == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (id_exists(taskenv, taskid) == FALSE)
        return emod_set(TMAN_ENOSUCHID);
    return column_moveid(taskid, tag);
}

int tman_id_cat(tman_ctx_t *ctx, char *env, char *id, struct tman_id_cat_opt *opt)
{
    int status;
    taskid = id ? id : column_getcid();
    taskenv = env ? env : column_getcenv();

    status = TMAN_OK;
    if (taskenv == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    else if (taskid == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (id_exists(taskenv, taskid) == FALSE)
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
    return column_addcenv(env);
}

int tman_env_del(tman_ctx_t *ctx, char *env, struct tman_env_del_opt *opt)
{
    taskenv = env ? env : column_getcenv();

    if (taskenv == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    else if (ermdir(tmanfs.base, taskenv)) {
        elog(1, "%s: could not delete env directory", taskenv);
        return emod_set(TMAN_NODEF_ERR);
    }
    return column_delcenv();
}

int tman_env_list(tman_ctx_t *ctx, struct tman_env_list_opt *opt)
{
    return TMAN_OK;
}

int tman_env_prev(tman_ctx_t *ctx, struct tman_env_prev_opt *opt)
{
    if (column_swapenv()) {
        return emod_set(TMAN_ENOPREVENV);
    }
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
    return column_addcenv(taskenv);
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
    while (ctx->units.pgn) {
        struct unitpgn *pgn = ctx->units.pgn;
        ctx->units.pgn = ctx->units.pgn->next;
        unit_delpgn(pgn);
    }
    free(ctx);
    return TMAN_OK;
}
