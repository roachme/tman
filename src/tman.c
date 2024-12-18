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
#include "config.h"

// TODO: Make NOT global.
struct tmanstruct tmanfs;

static int env_exists(char *env)
{
    char pathname[PATHSIZ + 1];
    sprintf(pathname, "%s/%s", tmanfs.base, env);
    return ISDIR(pathname);
}

static int id_exists(char *env, char *id)
{
    char pathname[PATHSIZ + 1];
    sprintf(pathname, "%s/%s/%s", tmanfs.base, env, id);
    return ISDIR(pathname);
}

static int tman_initfs()
{
    // TODO: Add a variable for task db as well.
    sprintf(tmanfs.base,   "%s",    config.base);
    sprintf(tmanfs.db,     "%s/%s", tmanfs.base, ".tman");
    sprintf(tmanfs.finit,  "%s/%s", tmanfs.db,   "inited");
    sprintf(tmanfs.fstate, "%s/%s", tmanfs.db,   "state");
    sprintf(tmanfs.pgn,    "%s/%s", tmanfs.base, ".pgn");
    sprintf(tmanfs.pgnins, "%s",    config.pgnins);
    return 0;
}

int core_init(const char *cmd)
{
    if (config_init())
        return elog(1, "failed to parse system config file");
    else if (tman_initfs())
        return elog(1, "could not create filesystem variables");
    else if (strncmp(cmd, "init", CMDSIZ) == 0
        || strncmp(cmd, "help", CMDSIZ) == 0
        || strncmp(cmd, "ver", CMDSIZ) == 0)
        return TMAN_OK;

    // TODO: Find a better way to check that util's inited.
    else if (ISFILE(tmanfs.finit) == FALSE)
        return elog(1, "not inited");
    else if (column_init())
        return elog(1, "column_init: error: could not init");
    return TMAN_OK;
}

int core_pwd()
{
    char *cid = column_getcid();
    printf("PWD: %s/%s/%s\n", tmanfs.base, column_getcenv(), cid ? cid : "");
    return TMAN_OK;
}

int core_id_add(char *env, char *id, struct tman_add_opt *opt)
{
    // TODO: Add support to pass unit values into unit_add()
    struct unitbin units[NKEYS] = {0};
    char *cenv = column_getcenv();
    opt->env = opt->env != NULL ? opt->env : cenv;

    if (opt->env == NULL)
        return elog(1, "no current environment");
    else if (!env_exists(opt->env))
        return elog(1, "%s: no such environment", opt->env);
    else if (!_chkid(id))
        return elog(1, "%s: illegal task id name", id);
    else if (id_exists(opt->env, id)) {
        if (opt->force == 0)
            elog(TMAN_ADD_IDEXT, "%s: task id already exists", id);
        return TMAN_ADD_IDEXT;
    }

    if (imkdir(tmanfs.base, opt->env, id) != 0)
        return elog(1, "%s: could not create task directory", id);
    else if (unit_addbin(opt->env, id, units) != 0)
        return elog(1, "%s: could not create task unit", id);
    else if (hookact("add", opt->env, id))
        return elog(1, "could not execute hook");
    else if (column_markid(id))
        return elog(1, "column_mark: failed");

    if (opt->noswitch == FALSE && strcmp(opt->env, cenv) == 0 && column_addcid(id) != 0)
        return elog(1, "column_addcid: failed");
    return TMAN_OK;
}

int core_id_del(char *env, char *id, struct tman_del_opt *opt)
{
    // FIXME: causes error when delete current task in previous env
    char *cid = column_getcid();
    char *pid = column_getpid();
    id  = id != NULL ? id : cid;
    opt->env = opt->env != NULL ? opt->env : column_getcenv();

    if (opt->env == NULL)
        return elog(1, "no current environment");
    else if (!env_exists(opt->env))
        return elog(1, "%s: no such environment", opt->env);
    else if (id == NULL)
        return elog(1, "no current task id");
    else if (!id_exists(opt->env, id)) {
        if (opt->force == 0)
            elog(TMAN_DEL_NOID, "%s: no such task id", id);
        return TMAN_DEL_NOID;
    }

    if (hookact("del", opt->env, id))
        return elog(1, "could not execute hook");
    else if (unit_delbin(opt->env, id))
        return elog(1, "%s: could not delete task unit", id);
    else if (irmdir(tmanfs.base, opt->env, id))
        return elog(1, "%s: could not delete task directory", id);
    else if (column_delspec(id))
        return elog(1, "%s: could not update special task IDs", id);
    return TMAN_OK;
}

int core_id_prev(void)
{
    if (column_swapid())
        return TMAN_ECORE;
    if (hookact("prev", column_getcenv(), column_getcid())) {
        return elog(TMAN_EHOOK, "could not execute hook");
    }
    return TMAN_OK;
}

int core_id_sync(void)
{
    char *cid  = column_getcid();
    char *cenv = column_getcenv();

    if (cid == NULL)
        return elog(TMAN_ECORE, "no current task id set");
    if (hookact("sync", cenv, cid))
        return elog(TMAN_EHOOK, "could not execute hook");
    return TMAN_OK;
}

int core_id_set(char *env, char *id, struct unitbin *unit)
{
    id  = id != NULL ? id : column_getcid();
    env = env != NULL ? env : column_getcenv();

    if (env == NULL)
        return elog(1, "no current environment");
    else if (!env_exists(env))
        return elog(1, "%s: no such environment", env);
    else if (id == NULL)
        return elog(1, "no current task id");
    else if (!id_exists(env, id))
        return elog(1, "%s: no such task id", id);
    else if (unit_setbin(env, id, unit))
        return elog(1, "%s: could not set unit values", id);

    // TODO: change task directory if id unit was changed
    // TODO: update task id status as well.
    return TMAN_OK;
}

// TODO: add support to switch to task in another environment.
int core_id_use(char *env, char *id, struct tman_use_opt *opt)
{
    opt->env = opt->env != NULL ? opt->env : column_getcenv();

    if (opt->env == NULL)
        return elog(1, "no current environment");
    if (!env_exists(opt->env))
        return elog(1, "%s: no such env", opt->env);
    else if (!_chkenv(opt->env))
        return elog(1, "%s: illegal task env name", opt->env);
    else if (id == NULL)
        return elog(1, "task id required");
    else if (!id_exists(opt->env, id))
        return elog(1, "cannot access '%s': no such task ID in env '%s'", id, opt->env);
    else if (opt->env != column_getcenv()) {
        fprintf(stderr, "trynna switch to task in another env\n");
        fprintf(stderr, "under development\n");
        return 1;
    }

    // TODO: it can't switch to task in non-current env.
    // Cuz it gotta switch env first.
    return column_addcid(id);
}

int core_id_move(char *id, char *dst, char *src)
{
    char dstid[BUFSIZ + 1] = {0};

    src = src ? src : column_getcenv();
    sprintf(dstid, "%s/%s/%s", tmanfs.base, dst, id);

    if (!env_exists(dst))
        return elog(1, "no such destination env");
    else if (!src || src == NULL)
        return elog(1, "no current env set");
    else if (!env_exists(src))
        return elog(1, "no such source env");
    else if (!id_exists(src, id))
        return elog(1, "cannot access '%s': no such task id", id);
    else if (id_exists(dst, id))
        return elog(1, "cannot move '%s': task id exists in env '%s'", id, dst);

    if (imove(tmanfs.base, id, dst, src))
        return elog(1, "%s: could not move task to %s", id, dst);
    else if (strcmp(column_getcid(), id) == 0)
        return column_delcid();
    else if (strcmp(column_getpid(), id) == 0)
        return !column_swapid() && !column_delcid();
    return TMAN_OK;
}

/*
 @param env char * | NULL (then list the current env)
 @return struct item * | NULL (if error happened)
*/
struct list *core_id_list(struct list *list, char *env)
{
    DIR *ids;
    size_t i = 0;
    struct dirent *ent;
    struct unitbin bunit[NKEYS];
    char pathname[PATHSIZ + 1];
    env = env ? env : column_getcenv();

    sprintf(pathname, "%s/%s", tmanfs.base, env);
    if (env == NULL) {
        elog(1, "no current environment set");
        return NULL;
    }
    else if (!env_exists(env)) {
        elog(1, "%s: no such environment", env);
        return NULL;
    }
    else if ((ids = opendir(pathname)) == NULL) {
        elog(1, "%s: could not read environment", env);
        return NULL;
    }

    while ((ent = readdir(ids)) != NULL && i < LSIZE) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        if (unit_getbin(bunit, env, ent->d_name) == NULL) {
            fprintf(stderr, "core_id_list %s: failde to get units\n", ent->d_name);
            continue;
        }

        /* TODO: remove check and warning because there might
         * case when no hooks executed or defined and it's ok.  */
        if (!hookls(list->ilist[i].pgn, env, ent->d_name)) {
            fprintf(stderr, "core_id_list %s: failed to get hookls output\n", ent->d_name);
        }
        strcpy(list->ilist[i].id, ent->d_name);
        strcpy(list->ilist[i].desc, bunit[4].val);
        list->ilist[i].col = column_getmark(list->ilist[i].id);
        ++i;
    }
    list->num = i;
    closedir(ids);
    return list;
}

int core_id_movecol(char *env, char *id, char *tag)
{
    id = id ? id : column_getcid();
    env = env ? env : column_getcenv();

    if (env == NULL)
        return elog(1, "no current env set");
    else if (id == NULL)
        return elog(1, "no current id set");
    else if (!id_exists(env, id))
        return elog(1, "%s: no such task id", id);
    return column_moveid(id, tag);
}

struct units *core_id_cat(char *env, char *id, struct units *units)
{
    id = id ? id : column_getcid();
    env = env ? env : column_getcenv();

    if (env == NULL) {
        elog(1, "no Current environment");
        return NULL;
    }
    else if (!env_exists(env)) {
        elog(1, "%s: no such env name", env);
        return NULL;
    }
    else if (id == NULL) {
        elog(1, "no current Task id");
        return NULL;
    }
    else if (!id_exists(env, id)) {
        elog(1, "%s: no such task id", id);
        return NULL;
    }

    // TODO: don't wanna lose plugin units if builtin ones
    // failed to parse. But gotta make user return value
    // make any sense for caller.

    /* No need to check return value because there might case
     * that no hooks are defined or executed */
    units->pgn = hookcat(units->pgn, env, id);
    if (unit_getbin(units->bin, env, id) == NULL)
        elog(1, "core_id_cat: failed to get builtn units");
    return units;
}

int core_env_add(char *env, struct tman_env_add_opt *opt)
{
    if (env == NULL)
        return elog(1, "env name required");
    else if (env_exists(env)) {
        if (opt->force == 0)
            elog(1, "%s: env already exists", env);
        return TMAN_ECORE;
    }
    else if (!_chkenv(env))
        return elog(1, "%s: illegal task env name", env);
    else if (emkdir(tmanfs.base, env))
        return elog(1, "%s: could not create env directory", env);
    return column_addcenv(env);
}

int core_env_del(char *env, struct tman_env_del_opt *opt)
{
    env = env ? env : column_getcenv();

    if (env == NULL)
        return elog(1, "no current env set");
    else if (!env_exists(env))
        return elog(1, "%s: no such env", env);
    else if (ermdir(tmanfs.base, env))
        return elog(1, "%s: could not delete env directory", env);
    return column_delcenv();
}

int core_env_list()
{
    return TMAN_OK;
}

int core_env_prev()
{
    return column_swapenv();
}

int core_env_use(char *env)
{
    if (env == NULL)
        return elog(1, "env name required");
    else if (!env_exists(env))
        return elog(1, "%s: env does not exist", env);
    return column_addcenv(env);
}

int core_isplugin(const char *pgn)
{
    return isplugin(pgn);
}

int core_plugin_exec(int argc, char **argv)
{
    return plugin(argc, argv);
}
