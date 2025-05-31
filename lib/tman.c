#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "col.h"
#include "dir.h"
#include "project.h"
#include "hook.h"
#include "unit.h"
#include "tman.h"
#include "tree.h"
#include "task.h"
#include "unit.h"
#include "link.h"
#include "common.h"
#include "osdep.h"
#include "errmod.h"

// TODO: Make NOT global.
struct tmanstruct tmanfs;

static int check_args(struct tman_arg *args)
{
    int status;

    if ((status = tman_check_arg_prj(args)))
        return status;
    else if ((status = tman_check_arg_brd(args)))
        return status;
    else if ((status = tman_check_arg_id(args)))
        return status;
    return status;
}

static struct tman_context *make_context(void)
{
    struct tman_context *ctx;

    if ((ctx = malloc(sizeof(struct tman_context))) == NULL)
        return NULL;

    memset(ctx, 0, sizeof(struct tman_context));
    return ctx;
}

static int fill_sysvars(struct tman_base *base)
{
    sprintf(tmanfs.base, "%s", base->task);
    sprintf(tmanfs.db, "%s/%s", base->task, ".tman");
    sprintf(tmanfs.finit, "%s/%s", tmanfs.db, "inited");
    sprintf(tmanfs.fstate, "%s/%s", tmanfs.db, "state");        // should be called prj
    sprintf(tmanfs.pgn, "%s/%s", base->task, ".pgn");   //should be called `pgndb`
    sprintf(tmanfs.pgnins, "%s", base->pgn);
    return LIBTMAN_OK;
}

int tman_check_arg_id(struct tman_arg *args)
{
    if (args->id == NULL && (args->id = task_curr(args->prj)) == NULL)
        return emod_set(LIBTMAN_ID_NOCURR);
    else if (task_is_valid_name(args->id) == FALSE)
        return emod_set(LIBTMAN_ID_ILLEG);
    else if (task_is_valid_length(args->id) == FALSE)
        return emod_set(LIBTMAN_ID_TOOLONG);
    else if (task_exist(args->prj, args->id) == FALSE)
        return emod_set(LIBTMAN_ID_NOSUCH);
    return LIBTMAN_OK;
}

int tman_check_arg_brd(struct tman_arg *args)
{
    /* TODO: add board name checks.  */
    return LIBTMAN_OK;
}

int tman_check_arg_prj(struct tman_arg *args)
{
    if (args->prj == NULL && (args->prj = project_getcurr()) == NULL)
        return emod_set(LIBTMAN_PRJ_NOCURR);
    else if (project_is_valid_name(args->prj) == FALSE)
        return emod_set(LIBTMAN_PRJ_ILLEG);
    else if (project_is_valid_length(args->prj) == FALSE)
        return emod_set(LIBTMAN_PRJ_TOOLONG);
    else if (project_exist(args->prj) == FALSE)
        return emod_set(LIBTMAN_PRJ_NOSUCH);
    return LIBTMAN_OK;
}

int tman_mkfs(void)
{
    if (MKDIR(tmanfs.base))
        return emod_set(LIBTMAN_SYS_FS);
    else if (MKDIR(tmanfs.base))
        return emod_set(LIBTMAN_SYS_FS);
    else if (MKDIR(tmanfs.pgn))
        return emod_set(LIBTMAN_SYS_FS);
    else if (MKDIR(tmanfs.pgnins))
        return emod_set(LIBTMAN_SYS_FS);
    else if (MKDIR(tmanfs.db))
        return emod_set(LIBTMAN_SYS_FS);
    else if (TOUCH(tmanfs.fstate))
        return emod_set(LIBTMAN_SYS_FS);
    else if (TOUCH(tmanfs.finit))
        return emod_set(LIBTMAN_SYS_FS);
    return LIBTMAN_OK;
}

struct tman_context *tman_init(struct tman_base *base)
{
    if (fill_sysvars(base)) {
        emod_set(LIBTMAN_ESYSVAR);
        return NULL;
    }
    return make_context();
}

int tman_setup(int setuplvl)
{
    int status = LIBTMAN_OK;

    if (setuplvl == LIBTMAN_SETUPSOFT)
        return LIBTMAN_OK;
    else if (setuplvl == LIBTMAN_SETUPHARD) {
        if ((status = tman_mkfs()) != LIBTMAN_OK) {
            status = emod_set(LIBTMAN_EINIT);
        }
    } else if (setuplvl == LIBTMAN_SETUPCHECK) {
        if (ISFILE(tmanfs.finit) != TRUE) {
            status = emod_set(LIBTMAN_EINIT);
        } else if (project_init(tmanfs.fstate) != 0)
            return emod_set(LIBTMAN_EINIT_PRJMOD);
        // roach: is it a good idea to init module column in general
        // and here?
    }
    return status;
}

int tman_pwd(void)
{
    char *prj, *id;

    if ((prj = project_getcurr()) == NULL)
        return emod_set(LIBTMAN_PRJ_NOCURR);
    if ((id = task_curr(prj)) == NULL)
        id = "";
    printf("PWD: %s/%s/%s\n", tmanfs.base, prj, id);
    return LIBTMAN_OK;
}

int tman_task_add(struct tman_context *ctx, struct tman_arg *args,
                  struct tman_option *options)
{
    int status;
    // TODO: Add support to pass unit values into unit_add()
    // TODO: maybe it's better to move units to ctx?

    /* Special case: task ID should not exists. If this's a case - let it go. */
    if ((status = check_args(args)) && status != LIBTMAN_ID_NOSUCH)
        return status;
    else if (task_exist(args->prj, args->id) == TRUE)
        return emod_set(LIBTMAN_ID_EXISTS);

    if (dir_id_add(tmanfs.base, args->prj, args->id) != 0)
        return emod_set(LIBTMAN_DIR_ID_MAKE);
    else if (unit_generate(args->prj, args->id))
        return emod_set(LIBTMAN_UNIT_MAKE);
    else if (task_add(args->prj, args->id))
        return emod_set(LIBTMAN_COL_ADD);
    else if (options->id_switch == TRUE
             && task_move(args->prj, args->id, COLCURR))
        return emod_set(LIBTMAN_COL_MOVE);
    return LIBTMAN_OK;
}

int tman_task_show(struct tman_context *ctx, struct tman_arg *args,
                   struct tman_option *options)
{
    int status;

    if ((status = check_args(args)))
        return status;

    strncpy(ctx->id, args->id, IDSIZ);
    if ((ctx->unitbin = unit_load(genpath_unit(args->prj, args->id))) == NULL)
        status = LIBTMAN_UNIT_GET;

    link_get_parent(args->prj, args->id, ctx->linkparent);
    link_get_child(args->prj, args->id, ctx->linkchild);

    if (ctx->linkparent[0] == '\0')
        strcpy(ctx->linkparent, "None");
    if (ctx->linkchild[0] == '\0')
        strcpy(ctx->linkchild, "None");
    return status;
}

int tman_task_col(struct tman_context *ctx, struct tman_arg *args, char *tag,
                  struct tman_option *options)
{
    int status;

    /* TODO: make task_move() failure more descriptive.  */

    if ((status = check_args(args)))
        return status;
    else if (col_ext(tag) == FALSE)
        return emod_set(LIBTMAN_COL_EXISTS);
    else if (task_move(args->prj, args->id, tag))
        return emod_set(LIBTMAN_COL_MOVE);
    return LIBTMAN_OK;
}

int tman_task_del(struct tman_context *ctx, struct tman_arg *args,
                  struct tman_option *options)
{
    int status;

    if ((status = check_args(args)))
        return status;

    if (task_del(args->prj, args->id))
        return emod_set(LIBTMAN_COL_DEL);
    else if (dir_id_del(tmanfs.base, args->prj, args->id))
        return emod_set(LIBTMAN_DIR_ID_DEL);
    return LIBTMAN_OK;
}

/*
 * roachme: refactor this shit.
 @param prj char * | NULL (then list the current project)
 @return struct item * | NULL (if error happened)
*/
int tman_task_list(struct tman_context *ctx, struct tman_arg *args,
                   struct tman_option *options)
{
    DIR *ids;
    int status;
    struct dirent *ent;
    struct tree *node;
    struct unit *units;

    // TODO: move to to cli part
    /* Free task ID list because it might be called more than once.  */

    node = NULL;
    units = NULL;

    if ((status = tman_check_arg_prj(args)) != LIBTMAN_OK)
        return status;
    else if ((ids = opendir(genpath_prj(args->prj))) == NULL)
        return emod_set(LIBTMAN_DIR_PRJ_OPEN);

    while ((ent = readdir(ids)) != NULL) {
        args->id = ent->d_name;

        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if (tman_check_arg_id(args)) {
            // TODO: roach: sholud we leave it here? If not then what?..
            continue;
        } else if ((units = unit_load(genpath_unit(args->prj, args->id))) ==
                   NULL) {
            // TODO: roach: sholud we leave it here? If not then what?..
            // IF builtin units could not get
            continue;
        }
        struct column column = col_getmark(args->prj, ent->d_name);
        node =
            tree_alloc(ent->d_name, col_prio(column.col),
                       unit_get(units, "desc"), "");
        ctx->ids = tree_add(ctx->ids, node);
    }
    closedir(ids);
    return LIBTMAN_OK;
}

/*
 * TODO: make changes atomic
 * TODO: make it possible to add multiple childs/parents to task
 */
int tman_task_link(struct tman_context *ctx, struct tman_arg *parent,
                   struct tman_arg *child, struct tman_option *options)
{
    int status;

    if ((status = check_args(parent)))
        return emod_set(status);
    else if ((status = check_args(child)))
        return emod_set(status);

    if (link_set_parent(parent->prj, parent->id, child->id))
        return emod_set(LIBTMAN_NODEF_ERR);
    if (link_set_child(parent->prj, parent->id, child->id))
        return emod_set(LIBTMAN_NODEF_ERR);
    return LIBTMAN_OK;
}

int tman_task_move(struct tman_context *ctx, struct tman_arg *src,
                   struct tman_arg *dst)
{
    int status;

    if ((status = check_args(src)))
        return emod_set(status);
    else if ((status = check_args(dst)) && status != LIBTMAN_ID_NOSUCH)
        return emod_set(status);
    else if (task_exist(dst->prj, dst->id))
        return emod_set(LIBTMAN_ID_EXISTS);

    /*
     * If move task from project to project, update current and previous tasks
     * in source project. Do it before moving task, cuz there might be
     * multiple current or previous tasks later.
     */
    if (strcmp(src->prj, dst->prj) != 0) {
        if (task_iscurr(src->prj, src->id))
            task_del(src->prj, src->id);
        else if (task_isprev(src->prj, src->id))
            task_del(src->prj, src->id);
    }

    if (dir_id_rename(tmanfs.base, src->prj, dst->prj, src->id, dst->id))
        return emod_set(LIBTMAN_DIR_ID_REN);
    return LIBTMAN_OK;
}

int tman_task_prev(struct tman_context *ctx, struct tman_arg *args,
                   struct tman_option *options)
{
    int status;

    if ((status = check_args(args)))
        return status;
    else if (task_prev(args->prj) == NULL)
        return emod_set(LIBTMAN_ID_NOPREV);

    if (task_swap(args->prj))
        return emod_set(LIBTMAN_ID_SWAP);

    /* Get new current task ID explicitly. check_args() won't work, cuz
     * there's args->id == old current task ID.  */
    args->id = task_curr(args->prj);
    return check_args(args);
}

struct unit *tman_unit_add(struct unit *head, char *key, char *val)
{
    return unit_add(head, key, val);
}

void *tman_unit_free(struct tman_context *ctx, struct tman_arg *args,
                     struct tman_option *options)
{
    unit_free(ctx->unitbin);
    return ctx->unitbin = NULL;
}

int tman_task_set(struct tman_context *ctx, struct tman_arg *args,
                  struct unit *unitbin, struct tman_option *options)
{
    int status;
    struct unit *item;
    struct unit *units;

    // TODO: move genpath_unit to a variable to not call it twice
    if ((status = check_args(args)))
        return status;
    else if ((units = unit_load(genpath_unit(args->prj, args->id))) == NULL)
        return emod_set(LIBTMAN_NODEF_ERR);

    for (item = unitbin; item; item = item->next) {
        unit_set(units, item->key, item->val);
    }
    if (unit_save(genpath_unit(args->prj, args->id), units))
        return emod_set(LIBTMAN_UNIT_SET);
    unit_free(units);
    return LIBTMAN_OK;
}

int tman_task_sync(struct tman_context *ctx, struct tman_arg *args,
                   struct tman_option *options)
{
    int status;

    if ((status = check_args(args)))
        return status;
    else if (options->id_switch == FALSE)
        return LIBTMAN_OK;      /* here's nothing to do.  */

    if (project_addcurr(args->prj))
        return emod_set(LIBTMAN_PRJ_SWITCH);
    else if (task_move(args->prj, args->id, COLCURR))
        return emod_set(LIBTMAN_ID_SWAP);
    return LIBTMAN_OK;
}

int tman_prj_add(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options)
{
    int status;

    /* Special case: project should not exists. If this's a case - let it go. */
    if ((status = tman_check_arg_prj(args)) && status != LIBTMAN_PRJ_NOSUCH)
        return status;
    else if (project_exist(args->prj) == TRUE)
        return emod_set(LIBTMAN_PRJ_EXISTS);

    if (dir_prj_add(tmanfs.base, args->prj))
        return emod_set(LIBTMAN_DIR_PRJ_MAKE);
    else if (unit_generate_prj(args->prj))
        return emod_set(LIBTMAN_UNIT_MAKE);
    else if (options->prj_switch == TRUE && project_addcurr(args->prj))
        return emod_set(LIBTMAN_PRJ_SWITCH);
    return LIBTMAN_OK;
}

int tman_prj_del(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options)
{
    int status;

    if ((status = tman_check_arg_prj(args)))
        return status;

    if (dir_prj_del(tmanfs.base, args->prj))
        return emod_set(LIBTMAN_DIR_PRJ_DEL);
    else if (project_is_curr(args->prj) == TRUE && project_delcurr())
        return emod_set(LIBTMAN_PRJ_DEL_CURR);
    else if (project_is_prev(args->prj) == TRUE && project_delprev())
        return emod_set(LIBTMAN_PRJ_DEL_PREV);
    return LIBTMAN_OK;
}

/*
 * roachme: Refactor this shit
*/
int tman_prj_list(struct tman_context *ctx, struct tman_option *options)
{
    DIR *edir;
    struct unit *units;
    struct dirent *ent;
    struct tree *node;
    int colprio = 1;
    char *cprj, *pprj;
    struct tman_arg args;
    char pgnout[PGNOUTSIZ + 1] = { 0 };

    if ((edir = opendir(tmanfs.base)) == NULL)
        return emod_set(LIBTMAN_DIR_PRJ_OPEN);

    /* Free task prj list because it might be called more than once.  */
    tree_free(ctx->prjs);
    ctx->prjs = NULL;

    cprj = project_getcurr();
    pprj = project_getprev();
    while ((ent = readdir(edir)) != NULL) {
        args.prj = ent->d_name;

        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if (tman_check_arg_prj(&args)) {
            // TODO: roach: sholud we leave it here? If not then what?..
            continue;
        } else if ((units = unit_load(genpath_unit_prj(args.prj))) == NULL) {
            // TODO: roach: sholud we leave it here? If not then what?..
            // IF builtin units could not get
            continue;
        }
        // TODO: unify this shit
        if (cprj != NULL && strncmp(cprj, ent->d_name, PRJSIZ) == 0)
            colprio = 1;
        else if (pprj != NULL && strncmp(pprj, ent->d_name, PRJSIZ) == 0)
            colprio = 2;
        else
            colprio = 3;
        node =
            tree_alloc(ent->d_name, colprio, unit_get(units, "desc"), pgnout);
        ctx->prjs = tree_add(ctx->prjs, node);
    }

    closedir(edir);
    return LIBTMAN_OK;
}

int tman_prj_prev(struct tman_context *ctx, struct tman_option *options)
{
    int status;
    struct tman_arg args;

    args.brd = args.id = NULL;
    if ((args.prj = project_getcurr()) && (status = tman_check_arg_prj(&args)))
        return status;
    if ((args.prj = project_getprev()) && (status = tman_check_arg_prj(&args)))
        return status;
    if (project_swap())
        return emod_set(LIBTMAN_PRJ_SWAP);
    return LIBTMAN_OK;
}

int tman_prj_rename(struct tman_context *ctx, struct tman_arg *src,
                    struct tman_arg *dst)
{
    int status;

    /* TODO: Use tman_check_arg_prj() to remove code duplication.  */
    if (src->prj == NULL)
        return emod_set(LIBTMAN_PRJ_MISSING);
    else if (dst->prj == NULL)
        return emod_set(LIBTMAN_PRJ_MISSING);
    else if ((status = tman_check_arg_prj(src)))
        return emod_set(status);
    else if ((status = tman_check_arg_prj(dst)) && status != LIBTMAN_PRJ_NOSUCH)
        return emod_set(status);
    else if (project_exist(dst->prj))
        return emod_set(LIBTMAN_PRJ_EXISTS);
    else if (project_is_valid_name(dst->prj) != TRUE)
        return emod_set(LIBTMAN_PRJ_ILLEG);
    else if (project_is_valid_length(dst->prj) == FALSE)
        return emod_set(LIBTMAN_PRJ_TOOLONG);

    if (project_is_curr(src->prj) == TRUE) {
        project_delcurr();
        project_addcurr(dst->prj);
    } else if (project_is_prev(src->prj) == TRUE) {
        project_delprev();
        project_addcurr(dst->prj);
        project_swap();
    }
    return dir_prj_rename(tmanfs.base, src->prj, dst->prj);
}

int tman_prj_set(struct tman_context *ctx, struct tman_arg *args,
                 struct unit *unitbin, struct tman_option *options)
{
    int status;
    struct unit *item;
    struct unit *units;

    if ((status = tman_check_arg_prj(args)))
        return status;
    else if ((units = unit_load(genpath_unit_prj(args->prj))) == NULL)
        return emod_set(LIBTMAN_NODEF_ERR);

    for (item = unitbin; item; item = item->next) {
        unit_set(units, item->key, item->val);
    }
    if (unit_save(genpath_unit_prj(args->prj), units))
        return emod_set(LIBTMAN_UNIT_SET);
    unit_free(units);
    return LIBTMAN_OK;
}

int tman_prj_sync(struct tman_context *ctx, struct tman_arg *args,
                  struct tman_option *options)
{
    int status;

    if ((status = tman_check_arg_prj(args)))
        return status;
    else if (options->prj_switch == FALSE)
        return LIBTMAN_OK;

    if (project_addcurr(args->prj))
        return emod_set(LIBTMAN_PRJ_SWITCH);
    return LIBTMAN_OK;
}

// TODO: move it to cli part
int tman_ispgn(char *pgndir, const char *pgname)
{
    return ispgn(pgndir, pgname);
}

// TODO: move it to cli part
int tman_pgnexec(struct tman_context *ctx, struct tman_arg *args, char *pgname,
                 char *pgncmd, struct tman_option *options)
{
    int status;

    if ((status = check_args(args)))
        return status;

    if (system(genpath_pgn(args->prj, args->id, pgname, pgncmd)))
        return emod_set(LIBTMAN_EPGN);
    return LIBTMAN_OK;
}

struct unit *tman_hook_show(struct tman_context *ctx, struct tman_hook *hooks,
                            struct tman_arg *args, char *cmd)
{
    // roach
    // todo: if no hooks are executed then what?
    ctx->unitpgn = hookshow(hooks, args->prj, args->id, cmd);
    return ctx->unitpgn;
}

int tman_hook_action(struct tman_context *ctx, struct tman_hook *hooks,
                     struct tman_arg *args, char *cmd)
{
    return hookact(hooks, cmd, args->prj, args->id);
}

/*
 * NOTE: pro'ly redundant cuz hook_action doesn't alloc memory
int tman_hook_action_free(struct tman_context *ctx, struct tman_arg *args,
                          char *cmd)
{
    return 0;
}
*/

struct unit *tman_hook_show_free(struct tman_context *ctx,
                                 struct tman_arg *args)
{
    unit_free(ctx->unitpgn);
    return NULL;
}

const char *tman_strerror(void)
{
    return emod_strerror();
}

struct tman_context *tman_deinit(struct tman_context *ctx)
{
    unit_free(ctx->unitbin);
    unit_free(ctx->unitpgn);
    tree_free(ctx->ids);
    tree_free(ctx->prjs);
    free(ctx);
    return NULL;
}
