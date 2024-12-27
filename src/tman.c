#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "col.h"
#include "dir.h"
#include "tman.h"
#include "env.h"
#include "tree.h"
#include "task.h"
#include "unit.h"
#include "hook.h"
#include "unit.h"
#include "common.h"
#include "osdep.h"
#include "errmod.h"
#include "config.h"

static int status;

// TODO: Make NOT global.
struct tmanstruct tmanfs;

static char *taskenv, *taskid;

static char task_currid[IDSIZ + 1], task_previd[IDSIZ + 1];
static char task_currenv[ENVSIZ + 1], task_prevenv[ENVSIZ + 1];

static int check_input_env(char *env)
{
    if ((taskenv = env) == NULL && (taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_isvalid(taskenv) == FALSE)
        return emod_set(TMAN_EILLEGENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    return 0;
}

static int check_input_id(char *id)
{
    if ((taskid = id) == NULL && (taskid = task_curr(taskenv)) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (task_chk(taskid) == FALSE)
        return emod_set(TMAN_EILLEGID);
    else if (task_ext(taskenv, taskid) == FALSE)
        return emod_set(TMAN_ENOSUCHID);
    return 0;
}

static int chkargs(char *env, char *id)
{
    if ((status = check_input_env(env)))
        return status;
    else if ((status = check_input_id(id)))
        return status;
    return 0;
}

static int foreach(struct tree *p, int (*func)())
{
    return 0;
}

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
    status = TMAN_OK;
    if (setuplvl == TMAN_SETUPSOFT)
        return TMAN_OK;
    else if (setuplvl == TMAN_SETUPHARD) {
        if ((status = tman_mkfs()) != TMAN_OK) {
            status = emod_set(TMAN_EINIT);
        }
    }
    else if (setuplvl == TMAN_SETUPCHECK) {
        if (ISFILE(tmanfs.finit) != TRUE) {
            status = emod_set(TMAN_EINIT);
        }
        else if (env_init(tmanfs.fstate) != 0)
            return emod_set(TMAN_EINIT_ENVMOD);
        // roach: is it a good idea to init module column in general
        // and here?
    }
    return status;
}

int tman_pwd()
{
    char *cid, *cenv;

    if ((cenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    if ((cid = task_curr(cenv)) == NULL)
        cid = "";
    printf("PWD: %s/%s/%s\n", tmanfs.base, cenv, cid);
    return TMAN_OK;
}

int tman_id_add(tman_ctx_t *ctx, char *env, char *id, struct tman_id_add_opt *opt)
{
    // TODO: Add support to pass unit values into unit_add()
    // TODO: maybe it's better to move units to ctx?
    struct unitbin units[NKEYS] = {0};
    char *cenv = env_getcurr();
    char *col = COLCURR;

    /* Special case: task ID should not exists. If this's a case - let it go. */
    if ((status = chkargs(env, id)) && status != TMAN_ENOSUCHID)
        return status;
    else if (task_ext(taskenv, taskid) == TRUE)
        return emod_set(TMAN_EIDEXISTS);
    else if (col_ext(col) == FALSE)
        return emod_set(TMAN_ECOLNEXIST);
    else if (unit_check(units) == FALSE)
        return emod_set(TMAN_EILLEGUNIT);

    if (imkdir(tmanfs.base, taskenv, taskid) != 0)
        return emod_set(TMAN_ETASKMKDIR);
    else if (unit_addbin(taskenv, taskid, units) != 0)
        return emod_set(TMAN_ETASKMKUNIT);
    else if (task_add(taskenv, taskid))
        return emod_set(TMAN_EADDID);
    else if (opt->noswitch == FALSE && task_move(taskenv, taskid, COLCURR))
        return emod_set(TMAN_EMOVEID);
    else if (hookact("add", taskenv, taskid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_del(tman_ctx_t *ctx, char *env, char *id, struct tman_id_del_opt *opt)
{
    // FIXME: causes error when delete current task in previous env
    char _taskid[IDSIZ + 1];

    if ((status = chkargs(env, id)))
        return status;

    /* Copy current task ID to another variable, cuz it'll be
     * lost once removed from column.  */
    strncpy(_taskid, taskid, IDSIZ);

    if (hookact("del", taskenv, taskid))
        return emod_set(TMAN_EHOOK);
    else if (unit_delbin(taskenv, taskid))
        return emod_set(TMAN_ETASKRMUNIT);
    else if (task_del(taskenv, taskid))
        return emod_set(TMAN_EDELID);
    else if (irmdir(tmanfs.base, taskenv, _taskid))
        return emod_set(TMAN_ETASKRMDIR);
    return TMAN_OK;
}

int tman_id_prev(tman_ctx_t *ctx, struct tman_id_prev_opt *opt)
{
    char *cenv;

    if ((cenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (task_curr(cenv) == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (task_prev(cenv) == NULL)
        return emod_set(TMAN_ENOPREVID);
    else if (task_swap(cenv))
        return emod_set(TMAN_ESWAPIDS);
    else if (hookact("prev", env_getcurr(), task_curr(cenv)))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_sync(tman_ctx_t *ctx, struct tman_id_sync_opt *opt)
{
    char *cid, *cenv;

    cenv = env_getcurr();
    cid = task_curr(cenv);
    if ((status = chkargs(cenv, cid)))
        return status;

    if (hookact("sync", cenv, cid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_set(tman_ctx_t *ctx, char *env, char *id, struct unitbin *unitbin, struct tman_id_set_opt *opt)
{
    if ((status = chkargs(env, id)))
        return status;

    if (unit_setbin(taskenv, taskid, unitbin))
        return emod_set(TMAN_EISETUNIT);

    // TODO: change task directory if id unit was changed
    // TODO: update task id status as well.
    return TMAN_OK;
}

int tman_id_use(tman_ctx_t *ctx, char *env, char *id, struct tman_id_use_opt *opt)
{
    if ((status = chkargs(env, id)))
        return status;

    /* switch to new current environment.  */
    if (strncmp(taskenv, env_getcurr(), ENVSIZ) != 0 && env_addcurr(taskenv) != 0)
        return emod_set(TMAN_ESWITCHENV);
    return task_move(taskenv, taskid, COLCURR);
}

int tman_id_move(tman_ctx_t *ctx, char *id, char *dst, char *src)
{
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
    taskenv = env;
    struct tree *node;

    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (env_isvalid(taskenv) == FALSE)
        return emod_set(TMAN_EILLEGENV);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENOSUCHENV);
    else if ((ids = opendir(genpath_env(taskenv))) == NULL)
        return emod_set(TMAN_ETASKOPENDIR);

    while ((ent = readdir(ids)) != NULL) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        if (unit_getbin(bunit, taskenv, ent->d_name) == NULL) {
            fprintf(stderr, "tman_id_list %s: failde to get units\n", ent->d_name);
            continue;
        }

        /* TODO: remove check and warning because there might
         * case when no hooks executed or defined and it's ok.  */
        //if (!hookls(ctx->list.ilist[i].pgn, taskenv, ent->d_name)) {
        /*
        if (!hookls(ctx->tree->pgnout, taskenv, ent->d_name)) {
            fprintf(stderr, "tman_id_list %s: failed to get hookls output\n", ent->d_name);
        }
        */
        struct column column = col_getmark(taskenv, ent->d_name);
        node = tree_alloc(ent->d_name, col_prio(column.tag), bunit[4].val, "");
        ctx->tree = tree_add(ctx->tree, node);
    }
    closedir(ids);
    return TMAN_OK;
}

int tman_id_col(tman_ctx_t *ctx, char *env, char *id, char *tag, struct tman_id_col_opt *opt)
{
    // TODO: does not change CWD if curr task was moved.
    // The problem in CLI command `col'.

    if ((status = chkargs(env, id)))
        return status;
    else if (col_ext(tag) == FALSE)
        return emod_set(TMAN_ECOLNEXIST);
    return task_move(taskenv, taskid, tag);
}

int tman_id_cat(tman_ctx_t *ctx, char *env, char *id, struct tman_id_cat_opt *opt)
{
    if ((status = chkargs(env, id)))
        return status;

    // TODO: don't wanna lose plugin units if builtin ones
    // failed to parse. But gotta make user return value
    // make any sense for caller.

    /* No need to check return value because there might case
     * that no hooks are defined or executed */
    ctx->units.pgn = hookcat(ctx->units.pgn, taskenv, taskid);
    if (unit_getbin(ctx->units.bin, taskenv, taskid) == NULL)
        status = emod_set(TMAN_EIGETUNIT);
    return status;
}

char *tman_id_getcurr(tman_ctx_t *ctx, char *env)
{
    taskenv = env;

    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL) {
        emod_set(TMAN_ENOCURRENV);
        return NULL;
    }
    else if ((taskid = task_curr(taskenv)) == NULL)
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
    else if ((taskid = task_prev(taskenv)) == NULL)
        return NULL;
    return strncpy(task_previd, taskid, IDSIZ);
}

int tman_env_add(tman_ctx_t *ctx, char *env, struct tman_env_add_opt *opt)
{
    if ((status = check_input_env(env)) && status != TMAN_ENOSUCHENV)
        return status;
    else if (env_exists(env) == TRUE)
        return emod_set(TMAN_EENVEXISTS);

    if (emkdir(tmanfs.base, taskenv))
        return emod_set(TMAN_ENVMKDIR);
    return env_addcurr(env);
}

int tman_env_del(tman_ctx_t *ctx, char *env, struct tman_env_del_opt *opt)
{
    if ((status = check_input_env(env)))
        return status;

    if (ermdir(tmanfs.base, taskenv))
        return emod_set(TMAN_ENVRMDIR);

    // TODO: check if that is a current env
    return env_delcurr();
}

int tman_env_list(tman_ctx_t *ctx, struct tman_env_list_opt *opt)
{
    DIR *edir;
    struct dirent *ent;
    struct tree *node;
    int colprio = 1;
    char *cenv, *penv;
    char *desc = "some env desc";
    char pgnout[PGNOUTSCSIZ + 1];

    if ((edir = opendir(tmanfs.base)) == NULL)
        return emod_set(TMAN_ENVOPENDIR);

    cenv = env_getcurr();
    penv = env_getprev();
    while ((ent = readdir(edir)) != NULL) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;

        // TODO: unify this shit
        if (cenv != NULL && strncmp(cenv, ent->d_name, ENVSIZ) == 0)
            colprio = 1;
        else if (penv != NULL && strncmp(penv, ent->d_name, ENVSIZ) == 0)
            colprio = 2;
        else
            colprio = 3;
        node = tree_alloc(ent->d_name, colprio, desc, pgnout);
        ctx->etree = tree_add(ctx->etree, node);
    }
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
    if ((status = check_input_env(env)))
        return status;
    return env_addcurr(taskenv);
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

int tman_plugin(tman_ctx_t *ctx, char *env, char *id, char *pgname, char *pgncmd, struct tman_pgn_opt *opt)
{
    return plugin(env, id, pgname, pgncmd);
}

const char *tman_strerror(void)
{
    return emod_strerror();
}

int tman_deinit(struct tman_context *ctx)
{
    unit_delpgn(ctx->units.pgn);
    tree_free(ctx->tree);
    tree_free(ctx->etree);
    free(ctx);
    return TMAN_OK;
}
