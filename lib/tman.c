#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "col.h"
#include "dir.h"
#include "project.h"
#include "hook.h"
#include "tman.h"
#include "tree.h"
#include "task.h"
#include "unit.h"
#include "common.h"
#include "osdep.h"
#include "errmod.h"

// roach: use this private structure
struct tman_struct;

// TODO: Make NOT global.
struct tmanstruct tmanfs;

/* Used by caller to get access to default values.  */
static char task_currid[IDSIZ + 1], task_previd[IDSIZ + 1];
static char task_currprj[PRJSIZ + 1], task_prevprj[PRJSIZ + 1];

static int check_args(struct tman_arg *args)
{
    int status;

    if ((status = tman_check_arg_id(args)))
        return status;
    else if ((status = tman_check_arg_prj(args)))
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
    sprintf(tmanfs.base, "%s", base->base);
    sprintf(tmanfs.db, "%s/%s", base->base, ".tman");
    sprintf(tmanfs.finit, "%s/%s", tmanfs.db, "inited");
    sprintf(tmanfs.fstate, "%s/%s", tmanfs.db, "state");        // should be called prj
    sprintf(tmanfs.pgn, "%s/%s", base->base, ".pgn");   //should be called `pgndb`
    sprintf(tmanfs.pgnins, "%s", base->pgn);
    return TMAN_OK;
}

/*
 * Get default value if it's not set.
 * @param args input arguments
*/
int tman_get_args(struct tman_arg *args)
{
    if (args->prj == NULL)
        args->prj = project_getcurr();
    if (args->prj && args->id == NULL)
        args->id = task_curr(args->prj);
    return TMAN_OK;
}

int tman_check_arg_id(struct tman_arg *args)
{
    if (args->id == NULL && (args->id = task_curr(args->prj)) == NULL)
        return emod_set(TMAN_ID_NOCURR);
    else if (task_chk(args->id) == FALSE)
        return emod_set(TMAN_ID_ILLEG);
    else if (task_ext(args->prj, args->id) == FALSE)
        return emod_set(TMAN_ID_NOSUCH);
    return TMAN_OK;
}

int tman_check_arg_prj(struct tman_arg *args)
{
    if (args->prj == NULL && (args->prj = project_getcurr()) == NULL)
        return emod_set(TMAN_PRJ_NOCURR);
    else if (is_project_valid(args->prj) == FALSE)
        return emod_set(TMAN_PRJ_ILLEG);
    else if (project_exist(args->prj) == FALSE)
        return emod_set(TMAN_PRJ_NOSUCH);
    return TMAN_OK;
}

int tman_mkfs(void)
{
    if (MKDIR(tmanfs.base))
        return emod_set(TMAN_SYS_FS);
    else if (MKDIR(tmanfs.base))
        return emod_set(TMAN_SYS_FS);
    else if (MKDIR(tmanfs.pgn))
        return emod_set(TMAN_SYS_FS);
    else if (MKDIR(tmanfs.pgnins))
        return emod_set(TMAN_SYS_FS);
    else if (MKDIR(tmanfs.db))
        return emod_set(TMAN_SYS_FS);
    else if (TOUCH(tmanfs.fstate))
        return emod_set(TMAN_SYS_FS);
    else if (TOUCH(tmanfs.finit))
        return emod_set(TMAN_SYS_FS);
    return TMAN_OK;
}

struct tman_context *tman_init(struct tman_base *base)
{
    if (fill_sysvars(base)) {
        emod_set(TMAN_ESYSVAR);
        return NULL;
    }
    return make_context();
}

int tman_setup(int setuplvl)
{
    int status = TMAN_OK;

    if (setuplvl == TMAN_SETUPSOFT)
        return TMAN_OK;
    else if (setuplvl == TMAN_SETUPHARD) {
        if ((status = tman_mkfs()) != TMAN_OK) {
            status = emod_set(TMAN_EINIT);
        }
    } else if (setuplvl == TMAN_SETUPCHECK) {
        if (ISFILE(tmanfs.finit) != TRUE) {
            status = emod_set(TMAN_EINIT);
        } else if (project_init(tmanfs.fstate) != 0)
            return emod_set(TMAN_EINIT_PRJMOD);
        // roach: is it a good idea to init module column in general
        // and here?
    }
    return status;
}

int tman_pwd(void)
{
    char *prj, *id;

    if ((prj = project_getcurr()) == NULL)
        return emod_set(TMAN_PRJ_NOCURR);
    if ((id = task_curr(prj)) == NULL)
        id = "";
    printf("PWD: %s/%s/%s\n", tmanfs.base, prj, id);
    return TMAN_OK;
}

int tman_id_add(struct tman_context *ctx, struct tman_arg *args,
                struct tman_option *options)
{
    int status;
    // TODO: Add support to pass unit values into unit_add()
    // TODO: maybe it's better to move units to ctx?
    struct unit units[NKEYS] = { 0 };

    /* Special case: task ID should not exists. If this's a case - let it go. */
    if ((status = check_args(args)) && status != TMAN_ID_NOSUCH)
        return status;
    else if (task_chklen(args->id) == FALSE)
        return emod_set(TMAN_ID_TOOLONG);
    else if (task_ext(args->prj, args->id) == TRUE)
        return emod_set(TMAN_ID_EXISTS);
    else if (unit_chkbin(units) == FALSE)
        return emod_set(TMAN_UNIT_ILLEG);

    if (dir_id_add(tmanfs.base, args->prj, args->id) != 0)
        return emod_set(TMAN_DIR_ID_MAKE);
    else if (unit_addbin(args->prj, args->id, units) != 0)
        return emod_set(TMAN_UNIT_MAKE);
    else if (task_add(args->prj, args->id))
        return emod_set(TMAN_COL_ADD);
    else if (options->id_switch == TRUE
             && task_move(args->prj, args->id, COLCURR))
        return emod_set(TMAN_COL_MOVE);
    return TMAN_OK;
}

int tman_id_show(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options)
{
    int status;

    if ((status = check_args(args)))
        return status;

    strncpy(ctx->units.id, args->id, IDSIZ);
    if (unit_getbin(ctx->units.bin, args->prj, args->id) == NULL)
        status = TMAN_UNIT_GET;
    return status;
}

int tman_id_col(struct tman_context *ctx, struct tman_arg *args, char *tag,
                struct tman_option *options)
{
    int status;
    // TODO: does not change CWD if curr task was moved.
    // The problem in CLI command `col'.

    if ((status = check_args(args)))
        return status;
    else if (col_ext(tag) == FALSE)
        return emod_set(TMAN_COL_EXISTS);
    return task_move(args->prj, args->id, tag);
}

int tman_id_del(struct tman_context *ctx, struct tman_arg *args,
                struct tman_option *options)
{
    int status;

    if ((status = check_args(args)))
        return status;

    /* FIXME: if current task gets deleted, plugin gun deletes
     * branches but won't switch to new current task branch.  */

    if (unit_delbin(args->prj, args->id))
        return emod_set(TMAN_UNIT_DEL);
    else if (task_del(args->prj, args->id))
        return emod_set(TMAN_COL_DEL);
    else if (dir_id_del(tmanfs.base, args->prj, args->id))
        return emod_set(TMAN_DIR_ID_DEL);
    return TMAN_OK;
}

int tman_id_find_by_desc(struct tman_context *ctx, struct tman_arg *args,
                         char *descpatt, struct tman_option *options)
{
    DIR *ids;
    int status;
    struct dirent *ent;
    struct unit bunit[NKEYS];
    struct tree *node;
    char *desc;

    /* Free task ID list because it might be called more than once.  */
    tree_free(ctx->ids);
    ctx->ids = NULL;

    if ((status = check_args(args)))
        return status;
    else if ((ids = opendir(genpath_prj(args->prj))) == NULL)
        return emod_set(TMAN_DIR_PRJ_OPEN);

    while ((ent = readdir(ids)) != NULL) {
        args->id = ent->d_name;

        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if (descpatt == NULL)
            continue;
        else if (tman_check_arg_id(args)) {
            // TODO: roach: sholud we leave it here? If not then what?..
            continue;
        } else if (unit_getbin(bunit, args->prj, ent->d_name) == NULL) {
            // TODO: roach: sholud we leave it here? If not then what?..
            // IF builtin units could not get
            continue;
        }
        desc = bunit[3].val;

        /* convert to lower case. */
        for (int i = 0; i < strlen(descpatt); ++i)
            descpatt[i] = tolower(descpatt[i]);

        char descbuf[BUFSIZ + 1];
        strncpy(descbuf, desc, BUFSIZ);
        for (int i = 0; i < strlen(descbuf); ++i)
            descbuf[i] = tolower(descbuf[i]);

        char *res;
        if ((res = strstr(descbuf, descpatt)) == NULL) {
            continue;
        }

        struct column column = col_getmark(args->prj, ent->d_name);
        node = tree_alloc(ent->d_name, col_prio(column.col), desc, "");
        ctx->ids = tree_add(ctx->ids, node);
    }
    closedir(ids);
    return TMAN_OK;
}

/*
 * roachme: refactor this shit.
 @param prj char * | NULL (then list the current project)
 @return struct item * | NULL (if error happened)
*/
int tman_id_list(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options)
{
    DIR *ids;
    int status;
    struct dirent *ent;
    struct unit bunit[NKEYS];
    struct tree *node;

    /* Free task ID list because it might be called more than once.  */
    ctx->ids = tree_free(ctx->ids);

    if ((status = tman_check_arg_prj(args)) != TMAN_OK)
        return status;
    else if ((ids = opendir(genpath_prj(args->prj))) == NULL)
        return emod_set(TMAN_DIR_PRJ_OPEN);

    while ((ent = readdir(ids)) != NULL) {
        args->id = ent->d_name;

        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if (tman_check_arg_id(args)) {
            // TODO: roach: sholud we leave it here? If not then what?..
            continue;
        } else if (unit_getbin(bunit, args->prj, ent->d_name) == NULL) {
            // TODO: roach: sholud we leave it here? If not then what?..
            // IF builtin units could not get
            continue;
        }
        struct column column = col_getmark(args->prj, ent->d_name);
        node = tree_alloc(ent->d_name, col_prio(column.col), bunit[3].val, "");
        ctx->ids = tree_add(ctx->ids, node);
    }
    closedir(ids);
    return TMAN_OK;
}

/* Link task IDs together: parent - children relationship.  */
int tman_id_link(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options)
{
    return 0;
}

int tman_id_move(struct tman_context *ctx, char *srcprj, char *dstprj,
                 char *srcid, char *dstid)
{
/*
    int status;
    char *_srcprj, *_srcid, *_dstprj, *_dstid;
    struct tman_arg args;

    if ((status = check_args(&args)) != TMAN_OK)
        return status;
    _srcprj = args.prj;
    _srcid = args.id;

    if (dstid == NULL)
        dstid = srcid;
    if ((status = check_args(dstprj, dstid)) && status != TMAN_ID_NOSUCH) {
        return status;
    }
    _dstprj = args->prj;
    _dstid = args->id;

    if (task_ext(_dstprj, _dstid) == TRUE) {
        return TMAN_ID_EXISTS;
    }

    // TODO: add support to change user's PWD

    // Rename task.
    if (strncmp(_srcprj, _dstprj, PRJSIZ) == 0) {
        if (dir_id_rename(tmanfs.base, _srcprj, _dstprj, _srcid, _dstid)) {
            return 1;
        }
    } else {
        // Move task to another project.
    }

    // roach: check this case
    // fprintf(stderr, "source and destination pathes are the same. Do nothing.");
    // TODO: update curr & prev if needed.
*/
    return 0;
}

int tman_id_prev(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options)
{
    int status;

    /* Check that current project and task ID are set.  */
    if (tman_get_args(args))
        return emod_set(TMAN_NODEF_ERR);
    if ((status = check_args(args)))
        return status;
    else if ((args->id = task_prev(args->prj)) == NULL)
        return emod_set(TMAN_ID_NOPREV);
    if (task_swap(args->prj))
        return emod_set(TMAN_ID_SWAP);

    return TMAN_OK;
}

int tman_id_set(struct tman_context *ctx, struct tman_arg *args,
                struct unit *unitbin, struct tman_option *options)
{
    int status;

    if ((status = check_args(args)))
        return status;
    else if (unit_chkbin(unitbin) == FALSE)
        return emod_set(TMAN_UNIT_ILLEG);

    if (unit_setbin(args->prj, args->id, unitbin))
        return emod_set(TMAN_UNIT_SET);
    return TMAN_OK;
}

int tman_id_sync(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options)
{
    int status;

    if ((status = check_args(args)))
        return status;

    if (options->id_switch == TRUE) {
        if (is_project_curr(args->prj) == FALSE
            && project_addcurr(args->prj) != 0)
            return emod_set(TMAN_PRJ_SWITCH);
        else if (task_iscurr(args->prj, args->id) == FALSE
                 && task_move(args->prj, args->id, COLCURR))
            return emod_set(TMAN_ID_SWAP);
    }
    return TMAN_OK;
}

char *tman_id_getcurr(struct tman_context *ctx, struct tman_arg *args)
{
    if (tman_check_arg_prj(args))
        return NULL;
    return strncpy(task_currid, args->id, IDSIZ);
}

char *tman_id_getprev(struct tman_context *ctx, struct tman_arg *args)
{
    if (check_args(args))
        return NULL;
    else if (task_prev(args->prj) == NULL)
        return NULL;
    return strncpy(task_previd, args->id, IDSIZ);
}

int tman_prj_add(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options)
{
    int status;

    /* Special case: project should not exists. If this's a case - let it go. */
    if ((status = tman_check_arg_prj(args)) && status != TMAN_PRJ_NOSUCH)
        return status;
    else if (project_exist(args->prj) == TRUE)
        return emod_set(TMAN_PRJ_EXISTS);
    else if (project_valid_length(args->prj) == FALSE)
        return emod_set(TMAN_PRJ_TOOLONG);

    if (dir_prj_add(tmanfs.base, args->prj))
        return emod_set(TMAN_DIR_PRJ_MAKE);
    else if (options->prj_switch == TRUE && project_addcurr(args->prj))
        return emod_set(TMAN_PRJ_SWITCH);
    return TMAN_OK;
}

int tman_prj_del(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options)
{
    int status;

    if ((status = tman_check_arg_prj(args)))
        return status;

    if (dir_prj_del(tmanfs.base, args->prj))
        return emod_set(TMAN_DIR_PRJ_DEL);
    else if (is_project_curr(args->prj) == TRUE && project_delcurr())
        return emod_set(TMAN_PRJ_DEL_CURR);
    else if (is_project_prev(args->prj) == TRUE && project_delprev())
        return emod_set(TMAN_PRJ_DEL_CURR);
    return TMAN_OK;
}

/*
 * roachme: Refactor this shit
*/
int tman_prj_list(struct tman_context *ctx, struct tman_option *options)
{
    DIR *edir;
    struct dirent *ent;
    struct tree *node;
    int colprio = 1;
    char *cprj, *pprj;
    struct tman_arg args;
    char *desc = "some project desc";
    char pgnout[PGNOUTSIZ + 1] = { 0 };

    if ((edir = opendir(tmanfs.base)) == NULL)
        return emod_set(TMAN_DIR_PRJ_OPEN);

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
        }
        // TODO: unify this shit
        if (cprj != NULL && strncmp(cprj, ent->d_name, PRJSIZ) == 0)
            colprio = 1;
        else if (pprj != NULL && strncmp(pprj, ent->d_name, PRJSIZ) == 0)
            colprio = 2;
        else
            colprio = 3;
        node = tree_alloc(ent->d_name, colprio, desc, pgnout);
        ctx->prjs = tree_add(ctx->prjs, node);
    }
    closedir(edir);
    return TMAN_OK;
}

int tman_prj_prev(struct tman_context *ctx, struct tman_option *options)
{
    int status;
    struct tman_arg args;

    if ((args.prj = project_getcurr()) && (status = tman_check_arg_prj(&args)))
        return status;
    if ((args.prj = project_getprev()) && (status = tman_check_arg_prj(&args)))
        return status;
    if (project_swap())
        return emod_set(TMAN_PRJ_SWAP);
    return TMAN_OK;
}

int tman_prj_set(struct tman_context *ctx, struct tman_arg *args,
                 struct tman_option *options)
{
    return TMAN_OK;
}

int tman_prj_sync(struct tman_context *ctx, struct tman_arg *args,
                  struct tman_option *options)
{
    int status;

    if ((status = tman_check_arg_prj(args)))
        return status;

    if (options->prj_switch == TRUE) {
        if (is_project_curr(args->prj) == FALSE
            && project_addcurr(args->prj) != 0)
            return emod_set(TMAN_PRJ_SWITCH);
    }
    return TMAN_OK;
}

char *tman_prj_getcurr(struct tman_context *ctx)
{
    char *prj;

    if ((prj = project_getcurr()) == NULL)
        return NULL;
    return strncpy(task_currprj, prj, PRJSIZ);
}

char *tman_prj_getprev(struct tman_context *ctx)
{
    char *prj;

    if ((prj = project_getprev()) == NULL)
        return NULL;
    return strncpy(task_prevprj, prj, PRJSIZ);
}

int tman_ispgn(char *pgndir, const char *pgname)
{
    return ispgn(pgndir, pgname);
}

int tman_pgnexec(struct tman_context *ctx, struct tman_arg *args, char *pgname,
                 char *pgncmd, struct tman_option *options)
{
    int status;

    if ((status = check_args(args)))
        return status;
    return system(genpath_pgn(args->prj, args->id, pgname, pgncmd));
}

struct unit *tman_hook_show(struct tman_context *ctx, struct tman_hook *hooks,
                            struct tman_arg *args, char *cmd)
{
    // todo: if no hooks are executed then what?
    ctx->units.pgn = hookshow(hooks, args->prj, args->id, cmd);
    return ctx->units.pgn;
}

int tman_hook_action(struct tman_context *ctx, struct tman_hook *hooks,
                     struct tman_arg *args, char *cmd)
{
    return hookact(hooks, cmd, args->prj, args->id);
}

int tman_hook_action_free(struct tman_context *ctx, struct tman_arg *args,
                          char *cmd)
{
    return 0;
}

struct unit *tman_hook_show_free(struct tman_context *ctx,
                                 struct tman_arg *args)
{
    unit_delpgn(ctx->units.pgn);
    return NULL;
}

const char *tman_strerror(void)
{
    return emod_strerror();
}

struct tman_context *tman_deinit(struct tman_context *ctx)
{
    unit_delpgn(ctx->units.pgn);
    tree_free(ctx->ids);
    tree_free(ctx->prjs);
    free(ctx);
    return NULL;
}
