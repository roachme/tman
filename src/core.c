#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "dir.h"
#include "tman.h"
#include "core.h"
#include "unit.h"
#include "hook.h"
#include "unit.h"
#include "common.h"
#include "column.h"
#include "osdep.h"

static int _envext(char *env)
{
    char pathname[PATHSIZ];
    sprintf(pathname, "%s/%s", tmanfs.base, env);
    return ISDIR(pathname);
}

static int _idext(char *env, char *id)
{
    char pathname[PATHSIZ];
    sprintf(pathname, "%s/%s/%s", tmanfs.base, env, id);
    return ISDIR(pathname);
}

int core_currdir()
{
    char *cid = column_getcid();
    printf("%s/%s/%s\n", tmanfs.base, column_getcenv(), cid ? cid : "");
    return 0;
}

int core_init(const char *cmd)
{
    if (strncmp(cmd, "init", CMDSIZ) == 0
        || strncmp(cmd, "help", CMDSIZ) == 0
        || strncmp(cmd, "ver", CMDSIZ) == 0)
        return 0;

    // TODO: Find a better way to check that util's inited.
    if (ISFILE(tmanfs.finit) == FALSE)
        return elog(1, "not inited");
    else if (column_init())
        return elog(1, "column_init: error: could not init");
    return 0;
}

int core_id_add(char *id, struct tman_add_opt *opt)
{
    // TODO: Add support to pass unit values into unit_add()
    char *cenv = column_getcenv();
    opt->env = opt->env != NULL ? opt->env : cenv;

    if (opt->env == NULL)
        return elog(1, "no current environment");
    else if (!_envext(opt->env))
        return elog(1, "%s: no such environment", opt->env);
    else if (!_chkid(id))
        return elog(1, "%s: illegal task id name", id);
    else if (_idext(opt->env, id)) {
        if (opt->force == 0)
            elog(TMAN_ADD_IDEXT, "%s: task id already exists", id);
        return TMAN_ADD_IDEXT;
    }

    if (imkdir(tmanfs.base, opt->env, id) != 0)
        return elog(1, "%s: could not create task directory", id);
    else if (unit_add(opt->env, id) != 0)
        return elog(1, "%s: could not create task unit", id);
    else if (hookact("add", opt->env, id))
        return elog(1, "could not execute hook");
    else if (column_markid(id))
        return elog(1, "column_mark: failed");

    if (opt->noswitch == FALSE && strcmp(opt->env, cenv) == 0 && column_addcid(id) != 0)
        return elog(1, "column_addcid: failed");
    return TMAN_OK;
}

int core_id_del(char *id, struct tman_del_opt *opt)
{
    // FIXME: causes error when delete current task in previous env
    char *cid = column_getcid();
    char *pid = column_getpid();
    id  = id != NULL ? id : cid;
    opt->env = opt->env != NULL ? opt->env : column_getcenv();

    if (opt->env == NULL)
        return elog(1, "no current environment");
    else if (!_envext(opt->env))
        return elog(1, "%s: no such environment", opt->env);
    else if (id == NULL)
        return elog(1, "no current task id");
    else if (!_idext(opt->env, id)) {
        if (opt->force == 0)
            elog(TMAN_DEL_NOID, "%s: no such task id", id);
        return TMAN_DEL_NOID;
    }

    if (hookact("del", opt->env, id))
        return elog(1, "could not execute hook");
    else if (unit_del(opt->env, id))
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

int core_id_set(char *env, char *id, struct bunit *unit)
{
    id  = id != NULL ? id : column_getcid();
    env = env != NULL ? env : column_getcenv();

    if (env == NULL)
        return elog(1, "no current environment");
    else if (!_envext(env))
        return elog(1, "%s: no such environment", env);
    else if (id == NULL)
        return elog(1, "no current task id");
    else if (!_idext(env, id))
        return elog(1, "%s: no such task id", id);
    else if (unit_set(env, id, unit))
        return elog(1, "%s: could not set unit values", id);

    // TODO: change task directory if id unit was changed
    // TODO: update task id status as well.
    return TMAN_OK;
}

// int core_id_add(char *id, struct tman_add_opt *opt)
int core_id_use(char *id, struct tman_use_opt *opt)
{
    opt->env = opt->env != NULL ? opt->env : column_getcenv();

    if (opt->env == NULL)
        return elog(1, "no current environment");
    if (!_envext(opt->env))
        return elog(1, "%s: no such env", opt->env);
    else if (!_chkenv(opt->env))
        return elog(1, "%s: illegal task env name", opt->env);
    else if (id == NULL)
        return elog(1, "task id required");
    else if (!_idext(opt->env, id))
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
    char dstid[BUFSIZ] = {0};

    src = src ? src : column_getcenv();
    sprintf(dstid, "%s/%s/%s", tmanfs.base, dst, id);

    if (!_envext(dst))
        return elog(1, "no such destination env");
    else if (!src || src == NULL)
        return elog(1, "no current env set");
    else if (!_envext(src))
        return elog(1, "no such source env");
    else if (!_idext(src, id))
        return elog(1, "cannot access '%s': no such task id", id);
    else if (_idext(dst, id))
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
    char pathname[PATHSIZ];
    char *cenv = column_getcenv();
    env = env ? env : cenv;
    sprintf(pathname, "%s/%s", tmanfs.base, env);
    DIR *ids = opendir(pathname);
    struct dirent *ent;

    if (env == NULL) {
        elog(1, "no current environment set");
        return NULL;
    }
    else if (!_envext(env)) {
        elog(1, "%s: no such environment", env);
        return NULL;
    }
    else if (!ids) {
        elog(1, "%s: could not read environment", env);
        return NULL;
    }

    // actual logic goes here
    size_t i = 0;
    struct bunit bunit;
    while ((ent = readdir(ids)) != NULL && i < LSIZE) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        if (unit_get(&bunit, env, ent->d_name) == NULL) {
            fprintf(stderr, "core_id_list %s: failde to get units\n", ent->d_name);
            continue;
        }
        if (!hookls(list->ilist[i].pgn, env, ent->d_name)) {
            fprintf(stderr, "core_id_list %s: failed to get hookls output\n", ent->d_name);
        }

        strcpy(list->ilist[i].id, ent->d_name);
        strcpy(list->ilist[i].desc, bunit.pair[4].val);
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
    else if (!_idext(env, id))
        return elog(1, "%s: no such task id", id);
    return column_moveid(id, tag);
}

struct units *core_id_cat(struct units *units, char *env, char *id)
{
    id = id ? id : column_getcid();
    env = env ? env : column_getcenv();

    if (env == NULL) {
        elog(1, "no Current environment");
        return NULL;
    }
    else if (!_envext(env)) {
        elog(1, "%s: no such env name", env);
        return NULL;
    }
    else if (id == NULL) {
        elog(1, "no current Task id");
        return NULL;
    }
    else if (!_idext(env, id)) {
        elog(1, "%s: no such task id", id);
        return NULL;
    }

    if (unit_get(&units->bin, env, id) == NULL) {
        elog(1, "core_id_cat: failed to get builtn units");
        return NULL;
    }
    if (hookcat(&units->pgn, env, id) == NULL) {
        elog(1, "core_id_cat: failed to get plugin units");
        return NULL;
    }
    return units;
}

int core_env_add(char *env, struct tman_env_add_opt *opt)
{
    if (env == NULL)
        return elog(1, "env name required");
    else if (_envext(env)) {
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
    else if (!_envext(env))
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
    else if (!_envext(env))
        return elog(1, "%s: env does not exist", env);
    return column_addcenv(env);
}
