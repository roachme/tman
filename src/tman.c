#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "col.h"
#include "dir.h"
#include "env.h"
#include "hook.h"
#include "tman.h"
#include "tree.h"
#include "task.h"
#include "unit.h"
#include "common.h"
#include "osdep.h"
#include "errmod.h"
#include "config.h"

/* Library function's return code */
static int status;

// TODO: Make NOT global.
struct tmanstruct tmanfs;

/* For inner use by API functions.  */
static char *taskenv, *taskid;

/* Used by caller to get access to default values.  */
static char task_currid[IDSIZ + 1], task_previd[IDSIZ + 1];
static char task_currenv[ENVSIZ + 1], task_prevenv[ENVSIZ + 1];


/*
 * Check that input environment name is valid
*/
static int check_input_env(char *env)
{
    if ((taskenv = env) == NULL && (taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENV_NOCURR);
    else if (env_isvalid(taskenv) == FALSE)
        return emod_set(TMAN_ENV_ILLEG);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENV_NOSUCH);
    return 0;
}

/*
 * Check that input task ID is valid
*/
static int check_input_id(char *id)
{
    if ((taskid = id) == NULL && (taskid = task_curr(taskenv)) == NULL)
        return emod_set(TMAN_ID_NOCURR);
    else if (task_chk(taskid) == FALSE)
        return emod_set(TMAN_ID_ILLEG);
    else if (task_ext(taskenv, taskid) == FALSE)
        return emod_set(TMAN_ID_NOSUCH);
    return 0;
}

/*
 * Check that input values, i.e environment name and task ID are valid
*/
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

static tman_ctx_t *make_context()
{
    tman_ctx_t *ctx;

    if ((ctx = malloc(sizeof(tman_ctx_t))) == NULL)
        return NULL;

    memset(ctx, 0, sizeof(tman_ctx_t));
    return ctx;
}

static int fill_sysvars()
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
    else if (fill_sysvars()) {
        emod_set(TMAN_ESYSVAR);
        return NULL;
    }
    return make_context();
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
    if ((taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENV_NOCURR);
    if ((taskid = task_curr(taskenv)) == NULL)
        taskid = "";
    printf("PWD: %s/%s/%s\n", tmanfs.base, taskenv, taskid);
    return TMAN_OK;
}

int tman_id_add(tman_ctx_t *ctx, char *env, char *id, struct tman_id_add_opt *opt)
{
    // TODO: Add support to pass unit values into unit_add()
    // TODO: maybe it's better to move units to ctx?
    struct unitbin units[NKEYS] = {0};
    char *col = COLCURR;

    /* Special case: task ID should not exists. If this's a case - let it go. */
    if ((status = chkargs(env, id)) && status != TMAN_ID_NOSUCH)
        return status;
    else if (task_ext(taskenv, taskid) == TRUE)
        return emod_set(TMAN_ID_EXISTS);
    else if (col_ext(col) == FALSE)
        return emod_set(TMAN_COL_EXISTS);
    else if (unit_check(units) == FALSE)
        return emod_set(TMAN_UNIT_ILLEG);

    if (dir_id_add(tmanfs.base, taskenv, taskid) != 0)
        return emod_set(TMAN_DIR_ID_MAKE);
    else if (unit_addbin(taskenv, taskid, units) != 0)
        return emod_set(TMAN_UNIT_MAKE);
    else if (task_add(taskenv, taskid))
        return emod_set(TMAN_COL_ADD);
    else if (opt->noswitch == FALSE && task_move(taskenv, taskid, COLCURR))
        return emod_set(TMAN_COL_MOVE);
    else if (hookact("add", taskenv, taskid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_del(tman_ctx_t *ctx, char *env, char *id, struct tman_id_del_opt *opt)
{
    if ((status = chkargs(env, id)))
        return status;

    if (hookact("del", taskenv, taskid))
        return emod_set(TMAN_EHOOK);
    else if (unit_delbin(taskenv, taskid))
        return emod_set(TMAN_UNIT_DEL);
    else if (task_del(taskenv, taskid))
        return emod_set(TMAN_COL_DEL);
    else if (dir_id_del(tmanfs.base, taskenv, taskid))
        return emod_set(TMAN_DIR_ID_DEL);
    return TMAN_OK;
}

int tman_id_prev(tman_ctx_t *ctx, struct tman_id_prev_opt *opt)
{
    /* Check that current environment and task ID are set.  */
    if ((status = chkargs(NULL, NULL)))
        return status;
    else if (task_prev(taskenv) == NULL)
        return emod_set(TMAN_ID_NOPREV);

    if (task_swap(taskenv))
        return emod_set(TMAN_ID_SWAP);
    else if (hookact("prev", env_getcurr(), task_curr(taskenv)))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_sync(tman_ctx_t *ctx, struct tman_id_sync_opt *opt)
{
    /* Check that current environment and task ID are set.  */
    if ((status = chkargs(NULL, NULL)))
        return status;

    if (hookact("sync", taskenv, taskid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_set(tman_ctx_t *ctx, char *env, char *id, struct unitbin *unitbin, struct tman_id_set_opt *opt)
{
    if ((status = chkargs(env, id)))
        return status;
    else if (unit_check(unitbin) == FALSE)
        return emod_set(TMAN_UNIT_ILLEG);

    if (unit_setbin(taskenv, taskid, unitbin))
        return emod_set(TMAN_UNIT_SET);
    return TMAN_OK;
}

int tman_id_use(tman_ctx_t *ctx, char *env, char *id, struct tman_id_use_opt *opt)
{
    if ((status = chkargs(env, id)))
        return status;

    /* If env not current then switch to it.  */
    if (env_iscurr(taskenv) == FALSE && env_addcurr(taskenv) != 0)
        return emod_set(TMAN_ENV_SWITCH);
    return task_move(taskenv, taskid, COLCURR);
}

int tman_id_move(tman_ctx_t *ctx, char *id, char *dst, char *src)
{
    return 0;
}

/*
 * roachme: refactor this shit.
 @param env char * | NULL (then list the current env)
 @return struct item * | NULL (if error happened)
*/
int tman_id_list(tman_ctx_t *ctx, char *env, struct tman_id_list_opt *opt)
{
    DIR *ids;
    struct dirent *ent;
    struct unitbin bunit[NKEYS];
    struct tree *node;
    char pgnout[PGNOUTSCSIZ + 1];

    if ((status = check_input_env(env)))
        return status;
    else if ((ids = opendir(genpath_env(taskenv))) == NULL)
        return emod_set(TMAN_DIR_ID_OPEN);

    while ((ent = readdir(ids)) != NULL) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        if (unit_getbin(bunit, taskenv, ent->d_name) == NULL) {
            fprintf(stderr, "tman_id_list %s: failde to get units\n", ent->d_name);
            continue;
        }

        hookls(pgnout, taskenv, ent->d_name);
        struct column column = col_getmark(taskenv, ent->d_name);
        node = tree_alloc(ent->d_name, col_prio(column.col), bunit[3].val, pgnout);
        ctx->tree = tree_add(ctx->tree, node);
        pgnout[0] = '\0';
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
        return emod_set(TMAN_COL_EXISTS);
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
        status = emod_set(TMAN_UNIT_GET);

    strncpy(ctx->units.id, taskid, IDSIZ);
    return status;
}

char *tman_id_getcurr(tman_ctx_t *ctx, char *env)
{
    if (chkargs(env, NULL))
        return NULL;
    return strncpy(task_currid, taskid, IDSIZ);
}

char *tman_id_getprev(tman_ctx_t *ctx, char *env)
{
    if (chkargs(env, task_prev(env)))
        return NULL;
    else if (task_prev(env) == NULL)
        return NULL;
    return strncpy(task_previd, taskid, IDSIZ);
}

int tman_env_add(tman_ctx_t *ctx, char *env, struct tman_env_add_opt *opt)
{
    if ((status = check_input_env(env)) && status != TMAN_ENV_NOSUCH)
        return status;
    else if (env_exists(env) == TRUE)
        return emod_set(TMAN_ENV_EXISTS);

    if (dir_env_add(tmanfs.base, taskenv))
        return emod_set(TMAN_DIR_ENV_MAKE);
    return env_addcurr(env);
}

int tman_env_del(tman_ctx_t *ctx, char *env, struct tman_env_del_opt *opt)
{
    if ((status = check_input_env(env)))
        return status;

    if (dir_env_del(tmanfs.base, taskenv))
        return emod_set(TMAN_DIR_ENV_DEL);
    else if (env_iscurr(taskenv) == TRUE && env_delcurr())
        return emod_set(TMAN_ENV_DEL_CURR);
    else if (env_isprev(taskenv) == TRUE && env_delprev())
        return emod_set(TMAN_ENV_DEL_CURR);
    return TMAN_OK;
}

/*
 * roachme: Refactor this shit
*/
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
        return emod_set(TMAN_DIR_ENV_OPEN);

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
    if ((status = check_input_env(NULL)))
        return status;
    else if (env_getprev() == NULL)
        return emod_set(TMAN_ENV_NOPREV);

    if (env_swapenvs())
        return emod_set(TMAN_ENV_SWAP);
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
    if ((taskenv = env_getcurr()) == NULL)
        return NULL;
    return strncpy(task_currenv, taskenv, ENVSIZ);
}

char *tman_env_getprev(tman_ctx_t *ctx)
{
    if ((taskenv = env_getprev()) == NULL)
        return NULL;
    return strncpy(task_prevenv, taskenv, ENVSIZ);
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
