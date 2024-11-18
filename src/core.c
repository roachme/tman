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
#include "state.h"
#include "common.h"
#include "osdep.h"

const char *usermarks = {
// mark tag level
    "& review 1",
};

static int _envext(char *env)
{
    char pathname[PATHSIZ];
    sprintf(pathname, "%s/%s", tmanfs.task, env);
    return !DCHK(pathname);
}

static int _idext(char *env, char *id)
{
    char pathname[PATHSIZ];
    sprintf(pathname, "%s/%s/%s", tmanfs.task, env, id);
    return !DCHK(pathname);
}

int core_currdir()
{
    printf("%s/%s/%s\n", tmanfs.task, state_getcenv(), state_getcid());
    return 0;
}

int core_init(const char *cmd)
{
    if (strncmp(cmd, "init", CMDSIZ) == 0
        || strncmp(cmd, "help", CMDSIZ) == 0
        || strncmp(cmd, "ver", CMDSIZ) == 0)
        return 0;

    // TODO: Find a better way to check that util's inited.
    if (FCHK(tmanfs.finit))
        return elog(1, "not inited");
    return state_init();
}

int core_id_add(char *id, struct tman_add_opt *opt)
{
    // TODO: Add support to pass unit values into unit_add()
    char *cenv = state_getcenv();
    opt->env = opt->env != NULL ? opt->env : cenv;

    if (opt->env[0] == '\0')
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

    if (imkdir(tmanfs.task, opt->env, id) != 0)
        return elog(1, "%s: could not create task directory", id);
    else if (unit_add(opt->env, id) != 0)
        return elog(1, "%s: could not create task unit", id);
    else if (hookact("add", opt->env, id))
        return elog(1, "could not execute hook");
    else if (strcmp(opt->env, cenv) == 0)
        return state_id_add(id);
    return TMAN_OK;
}

int core_id_del(char *id, struct tman_del_opt *opt)
{
    // FIXME: causes error when delete current task in previous env
    char *cid = state_getcid();
    char *pid = state_getpid();
    id  = id != NULL ? id : cid;
    opt->env = opt->env != NULL ? opt->env : state_getcenv();

    if (opt->env[0] == '\0')
        return elog(1, "no current environment");
    else if (!_envext(opt->env))
        return elog(1, "%s: no such environment", opt->env);
    else if (id[0] == '\0')
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
    else if (irmdir(tmanfs.task, opt->env, id))
        return elog(1, "%s: could not delete task directory", id);

    // TODO: simplify this
    if (strcmp(cid, id) == 0) {
        if (state_id_del() != 0)
            return elog(1, "could not delete current task id");
    }
    else if (strcmp(pid, id) == 0) {
        if (state_id_pdel() != 0)
            return elog(1, "could not delete previous task id");
    }

    return TMAN_OK;
}

int core_id_prev(void)
{
    char *cid  = state_getcid();
    char *cenv = state_getcenv();

    if (state_id_swap())
        return TMAN_ECORE;
    if (hookact("prev", cenv, cid))
        return elog(TMAN_EHOOK, "could not execute hook");
    return TMAN_OK;
}

int core_id_sync(void)
{
    char *cid  = state_getcid();
    char *cenv = state_getcenv();

    if (strlen(cid) == 0)
        return elog(TMAN_ECORE, "no current task id set");
    if (hookact("sync", cenv, cid))
        return elog(TMAN_EHOOK, "could not execute hook");
    return TMAN_OK;
}

int core_id_set(char *env, char *id, struct bunit *unit)
{
    id  = id != NULL ? id : state_getcid();
    env = env != NULL ? env : state_getcenv();

    if (env[0] == '\0')
        return elog(1, "no current environment");
    else if (!_envext(env))
        return elog(1, "%s: no such environment", env);
    else if (id[0] == '\0')
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
    opt->env = opt->env != NULL ? opt->env : state_getcenv();

    if (opt->env[0] == '\0')
        return elog(1, "no current environment");
    if (!_envext(opt->env))
        return elog(1, "%s: no such env", opt->env);
    else if (!_chkenv(opt->env))
        return elog(1, "%s: illegal task env name", opt->env);
    else if (id[0] == '\0')
        return elog(1, "task id required");
    else if (!_idext(opt->env, id))
        return elog(1, "cannot access '%s': no such task ID in env '%s'", id, opt->env);
    else if (opt->env != state_getcenv()) {
        fprintf(stderr, "trynna switch to task in another env\n");
        fprintf(stderr, "under development\n");
        return 1;
    }

    // TODO: it can't switch to task in non-current env.
    // Cuz it gotta switch env first.
    return state_id_add(id);
}

int core_id_move(char *id, char *dst, char *src)
{
    char dstid[BUFSIZ] = {0};

    src = src ? src : state_getcenv();
    sprintf(dstid, "%s/%s/%s", tmanfs.task, dst, id);

    if (!_envext(dst))
        return elog(1, "no such destination env");
    else if (!src || strlen(src) == 0)
        return elog(1, "no current env set");
    else if (!_envext(src))
        return elog(1, "no such source env");
    else if (!_idext(src, id))
        return elog(1, "cannot access '%s': no such task id", id);
    else if (_idext(dst, id))
        return elog(1, "cannot move '%s': task id exists in env '%s'", id, dst);

    if (imove(tmanfs.task, id, dst, src))
        return elog(1, "%s: could not move task to %s", id, dst);
    else if (strcmp(state_getcid(), id) == 0)
        return state_id_del();
    else if (strcmp(state_getpid(), id) == 0)
        return !state_id_swap() && !state_id_del();
    return TMAN_OK;
}

/*
 @param env char * | NULL (then list the current env)
 @return struct item * | NULL (if error happened)
*/
struct list *core_id_list(struct list *list, char *env)
{
    char pathname[PATHSIZ];
    char *cenv = state_getcenv();
    env = env ? env : cenv;
    sprintf(pathname, "%s/%s", tmanfs.task, env);
    DIR *ids = opendir(pathname);
    struct dirent *ent;

    if (strlen(env) == 0) {
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
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..") || ent->d_type != DT_DIR)
            continue;

        strcpy(list->ilist[i].id, ent->d_name);

        // struct bunit *unit_get(struct bunit *u, char *env, char *id);
        // TODO: looks disgusting
        // FIXME: gonna fail if no description is set
        // FIXME: gonna fail if description position changes
        if (unit_get(&bunit, env, list->ilist[i].id) == NULL) {
            elog(1, "core_id_list: failde to get units");
        }
        strcpy(list->ilist[i].desc, bunit.pair[4].val);

        // FIXME: if current and previous environments have the same task ID
        //        then task id might be marked as current/previous mistakenly.
        if (strcmp(env, cenv) == 0) {
            if (strcmp(list->ilist[i].id,      state_getcid()) == 0) {
                list->ilist[i].col.mark = '*';
                list->ilist[i].col.level = 0;
            }
            else if (strcmp(list->ilist[i].id, state_getpid()) == 0) {
                list->ilist[i].col.mark = '^';
                list->ilist[i].col.level = 1;
            }
            else {
                list->ilist[i].col.mark = '+';
                list->ilist[i].col.level = 2;
            }
        }
        // TODO: show previous env actual marks as well.
        // For right now module state has no means to provide current and previous
        // IDs of previous env.

        if (!hookls(list->ilist[i].pgn, env, list->ilist[i].id)) {
            elog(1, "failed to get hookls output");
        }
        ++i;
    }
    list->num = i;
    closedir(ids);
    return list;
}

struct units *core_id_cat(struct units *units, char *env, char *id)
{
    id = id ? id : state_getcid();
    env = env ? env : state_getcenv();

    if (!env) {
        elog(1, "no current environment");
        return NULL;
    }
    else if (!_envext(env)) {
        elog(1, "%s: no such env name", env);
        return NULL;
    }
    else if (!id || strlen(id) == 0) {
        elog(1, "no current task id");
        return NULL;
    }
    else if (!_idext(env, id)) {
        elog(1, "%s: no such task id", id);
        return NULL;
    }

    if (unit_get(&units->builtn, env, id) == NULL) {
        elog(1, "core_id_cat: failed to get builtn units");
        return NULL;
    }
    if (hookcat(&units->plugin, env, id) == NULL) {
        elog(1, "core_id_cat: failed to get plugin units");
        return NULL;
    }
    return units;
}

int core_env_add(char *env, struct tman_env_add_opt *opt)
{
    if (env[0] == '\0')
        return elog(1, "env name required");
    else if (_envext(env)) {
        if (opt->force == 0)
            elog(1, "%s: env already exists", env);
        return TMAN_ECORE;
    }
    else if (!_chkenv(env))
        return elog(1, "%s: illegal task env name", env);
    else if (emkdir(tmanfs.task, env))
        return elog(1, "%s: could not create env directory", env);
    return state_env_add(env);
}

int core_env_list()
{
    return TMAN_OK;
}

int core_env_prev()
{
    return state_env_swap();
}

int core_env_use(char *env)
{
    if (env == NULL)
        return elog(1, "env name required");
    else if (!_envext(env))
        return elog(1, "%s: env does not exist", env);
    return state_env_add(env);
}
