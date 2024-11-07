#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "dir.h"
#include "core.h"
#include "unit.h"
#include "hook.h"
#include "unit.h"
#include "state.h"
#include "common.h"
#include "osdep.h"

static int _envext(char *env)
{
    char pathname[PATHSIZ];
    sprintf(pathname, "%s/%s", TMANTASKS, env);
    return access(pathname, F_OK) == 0;
}

static int _idext(char *env, char *id)
{
    char pathname[PATHSIZ];
    sprintf(pathname, "%s/%s/%s", TMANTASKS, env, id);
    return access(pathname, F_OK) == 0;
}

int core_currdir()
{
    printf("%s/%s/%s\n", TMANTASKS, state_getcenv(), state_getcid());
    return 0;
}

int core_init(char *cmd)
{
    if (strncmp(cmd, "init", CMDSIZ) == 0
        || strncmp(cmd, "help", CMDSIZ) == 0
        || strncmp(cmd, "ver", CMDSIZ) == 0)
        return 0;

    if (FCHK(TMANINITED))
        return elog("not inited");
    return state_init();
}

int core_id_add(char *env, char *id)
{
    // TODO: Add support to pass unit values into unit_add()
    env = env != NULL ? env : state_getcenv();

    if (env[0] == '\0')
        return elog("no current environment");
    else if (!_envext(env))
        return elog("%s: no such environment", env);
    else if (!_chkid(id))
        return elog("%s: illegal task id name", id);
    else if (_idext(env, id))
        return elog("%s: task id already exists", id);

    if (imkdir(TMANTASKS, env, id) != 0)
        return elog("%s: could not create task directory", id);
    else if (unit_add(env, id) != 0)
        return elog("%s: could not create task unit", id);
    else if (hookact("add", env, id))
        return elog("could not execute hook");
    return state_id_add(id);
}

int core_id_del(char *env, char *id)
{
    id  = id != NULL ? id : state_getcid();
    env = env != NULL ? env : state_getcenv();

    if (env[0] == '\0')
        return elog("no current environment");
    else if (!_envext(env))
        return elog("%s: no such environment", env);
    else if (id[0] == '\0')
        return elog("no current task id");
    else if (!_idext(env, id))
        return elog("%s: no such task id", id);

    if (hookact("del", env, id))
        return elog("could not execute hook");
    else if (unit_del(env, id))
        return elog("%s: could not delete task unit", id);
    else if (irmdir(TMANTASKS, env, id))
        return elog("%s: could not delete task directory", id);

    // TODO: simplify this
    if (strcmp(state_getcid(), id) == 0) {
        if (state_id_del() != 0)
            return elog("could not delete current task id");
    }
    else if (strcmp(state_getpid(), id) == 0) {
        if (state_id_pdel() != 0)
            return elog("could not delete previous task id");
    }
    return 0;
}

int core_id_prev(void)
{
    return state_id_swap();
}

int core_id_sync(void)
{
    return !strlen(state_getcid());
}

int core_id_set(char *env, char *id, struct unit *unit)
{
    id  = id != NULL ? id : state_getcid();
    env = env != NULL ? env : state_getcenv();

    if (env[0] == '\0')
        return elog("no current environment");
    else if (!_envext(env))
        return elog("%s: no such environment", env);
    else if (id[0] == '\0')
        return elog("no current task id");
    else if (!_idext(env, id))
        return elog("%s: no such task id", id);
    else if (unit_set(env, id, unit))
        return elog("%s: could not set unit values", id);

    // TODO: change task directory if id unit was changed
    // TODO: update task id status as well.
    return 0;
}

int core_id_use(char *env, char *id)
{
    env = env != NULL ? env : state_getcenv();

    if (env[0] == '\0')
        return elog("no current environment");
    if (!_envext(env))
        return elog("%s: no such env", env);
    else if (!_chkenv(env))
        return elog("%s: illegal task env name", env);
    else if (id[0] == '\0')
        return elog("task id required");
    else if (!_idext(env, id))
        return elog("%s: no such task id", env);
    else if (env != state_getcenv())
        printf("trynna switch to task in another env\n");

    // TODO: it can't switch to task in non-current env.
    // Cuz it gotta switch env first.
    return state_id_add(id);
}

int core_id_move(char *id, char *dst, char *src)
{
    char dstid[BUFSIZ] = {0};

    src = src ? src : state_getcenv();
    sprintf(dstid, "%s/%s/%s", TMANTASKS, dst, id);

    if (!_envext(dst))
        return elog("no such destination env");
    else if (!src || strlen(src) == 0)
        return elog("no current env set");
    else if (!_envext(src))
        return elog("no such source env");
    else if (!_idext(src, id))
        return elog("cannot access '%s': no such task id", id);
    else if (!DCHK(dstid))
        return elog("cannot move '%s': task id exists in env '%s'", id, dst);

    if (imove(TMANTASKS, id, dst, src))
        return elog("%s: could not move task to %s", id, dst);
    else if (strcmp(state_getcid(), id) == 0)
        return state_id_del();
    else if (strcmp(state_getpid(), id) == 0)
        return !state_id_swap() && !state_id_del();
    return 0;
}

/*
 @param env char * | NULL (then list the current env)
 @return struct item * | NULL (if error happened)
*/
struct list *core_id_list(struct list *list, char *env)
{
    char pathname[PATHSIZ];
    env = env ? env : state_getcenv();
    sprintf(pathname, "%s/%s", TMANTASKS, env);
    DIR *ids = opendir(pathname);
    struct dirent *ent;

    if (strlen(env) == 0) {
        elog("no current environment set");
        return NULL;
    }
    else if (!_envext(env)) {
        elog("%s: no such environment", env);
        return NULL;
    }
    else if (!ids) {
        elog("%s: could not read environment", env);
        return NULL;
    }

    // actual logic goes here
    size_t i = 0;
    while ((ent = readdir(ids)) != NULL && i < LSIZE) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
            continue;

        strcpy(list->ilist[i].id, ent->d_name);
        strcpy(list->ilist[i].desc, "dumb description");

        // struct bunit *unit_get(struct bunit *u, char *env, char *id);
        // TODO: looks disgusting
        // FIXME: gonna fail if no description is set
        // FIXME: gonna fail if description position changes
        struct bunit bunit;
        if (unit_get(&bunit, env, list->ilist[i].id) == NULL) {
            elog("core_id_list: failde to get units");
        }
        strcpy(list->ilist[i].desc, bunit.pair[4].val);

        // FIXME: if current and previous environments have the same task ID
        //        then task id might be marked as current/previous mistakenly.
        if (strcmp(env, state_getcenv()) == 0) {
            if (strcmp(list->ilist[i].id,      state_getcid()) == 0)
                list->ilist[i].mark = '*';
            else if (strcmp(list->ilist[i].id, state_getpid()) == 0)
                list->ilist[i].mark = '^';
            else
                list->ilist[i].mark = '+';
        }
        else if (strcmp(env, state_getcenv()) == 0) {
            if (strcmp(list->ilist[i].id,      state_getcid()) == 0)
                list->ilist[i].mark = '*';
            else if (strcmp(list->ilist[i].id, state_getpid()) == 0)
                list->ilist[i].mark = '^';
            else
                list->ilist[i].mark = '+';
        }
        else
            list->ilist[i].mark = '+';

        // handle hookls
        if (!hookls(list->ilist[i].pgn, env, list->ilist[i].id)) {
            elog("failed to get hookls output");
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
        elog("no current environment");
        return NULL;
    }
    else if (!_envext(env)) {
        elog("%s: no such env name", env);
        return NULL;
    }
    else if (!id || strlen(id) == 0) {
        elog("no current task id");
        return NULL;
    }
    else if (!_idext(env, id)) {
        elog("%s: no such task id", id);
        return NULL;
    }

    if (unit_get(&units->builtn, env, id) == NULL) {
        elog("core_id_cat: failed to get builtn units");
        return NULL;
    }
    if (hookcat(&units->plugin, env, id) == NULL) {
        elog("core_id_cat: failed to get plugin units");
        return NULL;
    }
    return units;
}

int core_env_add(char *env)
{
    if (env[0] == '\0')
        return elog("env name required");
    else if (_envext(env))
        return elog("%s: env already exists", env);
    else if (!_chkenv(env))
        return elog("%s: illegal task env name", env);
    else if (emkdir(TMANTASKS, env))
        return elog("%s: could not create env directory", env);
    return state_env_add(env);
}

int core_env_list()
{
    return 0;
}

int core_env_prev()
{
    return state_env_swap();
}

int core_env_use(char *env)
{
    if (env[0] == '\0')
        return elog("env name required");
    else if (!_envext(env))
        return elog("%s: env does not exist", env);
    return state_env_add(env);
}
