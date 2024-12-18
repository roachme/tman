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

int tman_init(const char *cmd)
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

int tman_pwd()
{
    char *cid = column_getcid();
    printf("PWD: %s/%s/%s\n", tmanfs.base, column_getcenv(), cid ? cid : "");
    return TMAN_OK;
}

int tman_id_add(char *env, char *id, struct tman_id_add_opt *opt)
{
    // TODO: Add support to pass unit values into unit_add()
    struct unitbin units[NKEYS] = {0};
    char *cenv = column_getcenv();
    env = env != NULL ? env : cenv;

    if (env == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (!env_exists(env)) {
        elog(1, "teetet '%s'", env);
        return emod_set(TMAN_ENOSUCHENV);
    }
    else if (!_chkid(id))
        return emod_set(TMAN_EILLEGID);
    else if (id_exists(env, id))
        return emod_set(TMAN_EIDEXISTS);

    if (imkdir(tmanfs.base, env, id) != 0)
        return emod_set(TMAN_ETASKMKDIR);
    else if (unit_addbin(env, id, units) != 0)
        return emod_set(TMAN_ETASKMKUNIT);
    else if (hookact("add", env, id))
        return emod_set(TMAN_EHOOK);
    else if (column_markid(id)) {
        elog(1, "column_mark: failed");
        return emod_set(TMAN_NODEF_ERR);
    }
    if (opt->noswitch == FALSE && strcmp(env, cenv) == 0 && column_addcid(id) != 0) {
        elog(1, "column_addcid: failed");
        return emod_set(TMAN_NODEF_ERR);
    }
    return TMAN_OK;
}

int tman_id_del(char *env, char *id, struct tman_id_del_opt *opt)
{
    // FIXME: causes error when delete current task in previous env
    char *cid = column_getcid();
    char *pid = column_getpid();
    id  = id != NULL ? id : cid;
    env = env != NULL ? env : column_getcenv();

    if (env == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (!env_exists(env))
        return emod_set(TMAN_ENOSUCHENV);
    else if (id == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (!id_exists(env, id))
        return emod_set(TMAN_ENOSUCHID);

    if (hookact("del", env, id))
        return emod_set(TMAN_EHOOK);
    else if (unit_delbin(env, id))
        return emod_set(TMAN_ETASKRMUNIT);
    else if (irmdir(tmanfs.base, env, id))
        return emod_set(TMAN_ETASKRMDIR);
    else if (column_delspec(id)) {
        elog(1, "%s: could not update special task IDs", id);
        return emod_set(TMAN_NODEF_ERR);
    }
    return TMAN_OK;
}

int tman_id_prev(void)
{
    if (column_swapid())
        return emod_set(TMAN_ENOPREVID);
    if (hookact("prev", column_getcenv(), column_getcid()))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_sync(void)
{
    char *cid  = column_getcid();
    char *cenv = column_getcenv();

    if (cid == NULL)
        return emod_set(TMAN_ENOCURRID);
    if (hookact("sync", cenv, cid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_set(char *env, char *id, struct unitbin *unit)
{
    id  = id != NULL ? id : column_getcid();
    env = env != NULL ? env : column_getcenv();

    if (env == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (!env_exists(env))
        return emod_set(TMAN_ENOSUCHENV);
    else if (id == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (!id_exists(env, id))
        return emod_set(TMAN_ENOSUCHID);
    else if (unit_setbin(env, id, unit)) {
        elog(1, "%s: could not set unit values", id);
        return emod_set(TMAN_NODEF_ERR);
    }

    // TODO: change task directory if id unit was changed
    // TODO: update task id status as well.
    return TMAN_OK;
}

// TODO: add support to switch to task in another environment.
int tman_id_use(char *env, char *id, struct tman_id_use_opt *opt)
{
    env = env != NULL ? env : column_getcenv();

    if (env == NULL)
        return emod_set(TMAN_ENOCURRENV);
    if (!env_exists(env))
        return emod_set(TMAN_ENOSUCHENV);
    else if (!_chkenv(env))
        return emod_set(TMAN_EILLEGENV);
    else if (id == NULL)
        return emod_set(TMAN_EREQRID);
    else if (!id_exists(env, id))
        return emod_set(TMAN_EMISSID);
    else if (env != column_getcenv()) {
        fprintf(stderr, "trynna switch to task in another env\n");
        fprintf(stderr, "under development\n");
        return emod_set(TMAN_NODEF_ERR);
    }

    // TODO: it can't switch to task in non-current env.
    // Cuz it gotta switch env first.
    return column_addcid(id);
}

int tman_id_move(char *id, char *dst, char *src)
{
    char dstid[BUFSIZ + 1] = {0};

    src = src ? src : column_getcenv();
    sprintf(dstid, "%s/%s/%s", tmanfs.base, dst, id);

    if (!env_exists(dst)) {
        elog(1, "no such destination env");
        return emod_set(TMAN_NODEF_ERR);
    }
    else if (!src || src == NULL) {
        elog(1, "no current env set");
        return emod_set(TMAN_NODEF_ERR);
    }
    else if (!env_exists(src)) {
        elog(1, "no such source env");
        return emod_set(TMAN_NODEF_ERR);
    }
    else if (!id_exists(src, id))
        emod_set(TMAN_ENOSUCHID);
    else if (id_exists(dst, id)) {
        elog(1, "cannot move '%s': task id exists in env '%s'", id, dst);
        return emod_set(TMAN_NODEF_ERR);
    }

    if (imove(tmanfs.base, id, dst, src)) {
        elog(1, "%s: could not move task to %s", id, dst);
        return emod_set(TMAN_NODEF_ERR);
    }
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
struct list *tman_id_list(struct list *list, char *env)
{
    DIR *ids;
    size_t i = 0;
    struct dirent *ent;
    struct unitbin bunit[NKEYS];
    char pathname[PATHSIZ + 1];
    env = env ? env : column_getcenv();

    sprintf(pathname, "%s/%s", tmanfs.base, env);
    if (env == NULL) {
        emod_set(TMAN_ENOCURRENV);
        return NULL;
    }
    else if (!env_exists(env)) {
        emod_set(TMAN_ENOSUCHENV);
        return NULL;
    }
    else if ((ids = opendir(pathname)) == NULL) {
        elog(1, "%s: could not read environment", env);
        emod_set(TMAN_NODEF_ERR);
        return NULL;
    }

    while ((ent = readdir(ids)) != NULL && i < LSIZE) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        if (unit_getbin(bunit, env, ent->d_name) == NULL) {
            fprintf(stderr, "tman_id_list %s: failde to get units\n", ent->d_name);
            continue;
        }

        /* TODO: remove check and warning because there might
         * case when no hooks executed or defined and it's ok.  */
        if (!hookls(list->ilist[i].pgn, env, ent->d_name)) {
            fprintf(stderr, "tman_id_list %s: failed to get hookls output\n", ent->d_name);
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

int tman_id_col(char *env, char *id, char *tag)
{
    id = id ? id : column_getcid();
    env = env ? env : column_getcenv();

    if (env == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (id == NULL)
        return emod_set(TMAN_ENOCURRID);
    else if (!id_exists(env, id))
        return emod_set(TMAN_ENOSUCHID);
    return column_moveid(id, tag);
}

struct units *tman_id_cat(char *env, char *id, struct units *units)
{
    id = id ? id : column_getcid();
    env = env ? env : column_getcenv();

    if (env == NULL) {
        emod_set(TMAN_ENOCURRENV);
        return NULL;
    }
    else if (!env_exists(env)) {
        elog(1, "%s: no such env name", env);
        return NULL;
    }
    else if (id == NULL) {
        emod_set(TMAN_ENOCURRID);
        return NULL;
    }
    else if (!id_exists(env, id)) {
        emod_set(TMAN_ENOSUCHID);
        return NULL;
    }

    // TODO: don't wanna lose plugin units if builtin ones
    // failed to parse. But gotta make user return value
    // make any sense for caller.

    /* No need to check return value because there might case
     * that no hooks are defined or executed */
    units->pgn = hookcat(units->pgn, env, id);
    if (unit_getbin(units->bin, env, id) == NULL)
        elog(1, "tman_id_cat: failed to get builtn units");
    return units;
}

int tman_env_add(char *env, struct tman_env_add_opt *opt)
{
    if (env == NULL)
        return emod_set(TMAN_EREQRENV);
    else if (env_exists(env))
        return emod_set(TMAN_EENVEXISTS);
    else if (!_chkenv(env))
        return emod_set(TMAN_EILLEGENV);
    else if (emkdir(tmanfs.base, env)) {
        elog(1, "%s: could not create env directory", env);
        return emod_set(TMAN_NODEF_ERR);
    }
    return column_addcenv(env);
}

int tman_env_del(char *env, struct tman_env_del_opt *opt)
{
    env = env ? env : column_getcenv();

    if (env == NULL)
        return emod_set(TMAN_ENOCURRENV);
    else if (!env_exists(env))
        return emod_set(TMAN_ENOSUCHENV);
    else if (ermdir(tmanfs.base, env)) {
        elog(1, "%s: could not delete env directory", env);
        return emod_set(TMAN_NODEF_ERR);
    }
    return column_delcenv();
}

int core_env_list()
{
    return TMAN_OK;
}

int tman_env_prev()
{
    return column_swapenv();
}

int tman_env_use(char *env)
{
    if (env == NULL)
        return emod_set(TMAN_EREQRENV);
    else if (!env_exists(env))
        return emod_set(TMAN_ENOSUCHENV);
    return column_addcenv(env);
}

int tman_isplugin(const char *pgn)
{
    return isplugin(pgn);
}

int tman_plugin(int argc, char **argv)
{
    return plugin(argc, argv);
}

const char *tman_strerror(void)
{
    return emod_strerror();
}
