#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include "col.h"
#include "dir.h"
#include "prj.h"
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
static char *taskprj, *taskid;

/* Used by caller to get access to default values.  */
static char task_currid[IDSIZ + 1], task_previd[IDSIZ + 1];
static char task_currprj[PRJSIZ + 1], task_prevprj[PRJSIZ + 1];


/*
 * Check that input project name is valid
*/
static int check_input_prj(char *prj)
{
    if ((taskprj = prj) == NULL && (taskprj = prj_getcurr()) == NULL)
        return emod_set(TMAN_PRJ_NOCURR);
    else if (prj_isvalid(taskprj) == FALSE)
        return emod_set(TMAN_PRJ_ILLEG);
    else if (prj_exists(taskprj) == FALSE)
        return emod_set(TMAN_PRJ_NOSUCH);
    return 0;
}

/*
 * Check that input task ID is valid
*/
static int check_input_id(char *id)
{
    if ((taskid = id) == NULL && (taskid = task_curr(taskprj)) == NULL)
        return emod_set(TMAN_ID_NOCURR);
    else if (task_chk(taskid) == FALSE)
        return emod_set(TMAN_ID_ILLEG);
    else if (task_ext(taskprj, taskid) == FALSE)
        return emod_set(TMAN_ID_NOSUCH);
    return TMAN_OK;
}

/*
 * Check that input values, i.e project name and task ID are valid
*/
static int chkargs(char *prj, char *id)
{
    if ((status = check_input_prj(prj)))
        return status;
    else if ((status = check_input_id(id)))
        return status;
    return TMAN_OK;
}

static tman_ctx_t *make_context(void)
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
        else if (prj_init(tmanfs.fstate) != 0)
            return emod_set(TMAN_EINIT_PRJMOD);
        // roach: is it a good idea to init module column in general
        // and here?
    }
    return status;
}

int tman_pwd(void)
{
    if ((taskprj = prj_getcurr()) == NULL)
        return emod_set(TMAN_PRJ_NOCURR);
    if ((taskid = task_curr(taskprj)) == NULL)
        taskid = "";
    printf("PWD: %s/%s/%s\n", tmanfs.base, taskprj, taskid);
    return TMAN_OK;
}

int tman_id_add(tman_ctx_t *ctx, char *prj, char *id, struct tman_id_add_opt *opt)
{
    // TODO: Add support to pass unit values into unit_add()
    // TODO: maybe it's better to move units to ctx?
    struct unit units[NKEYS] = {0};

    /* Special case: task ID should not exists. If this's a case - let it go. */
    if ((status = chkargs(prj, id)) && status != TMAN_ID_NOSUCH)
        return status;
    else if (task_chklen(taskid) == FALSE)
        return emod_set(TMAN_ID_TOOLONG);
    else if (task_ext(taskprj, taskid) == TRUE)
        return emod_set(TMAN_ID_EXISTS);
    else if (unit_chkbin(units) == FALSE)
        return emod_set(TMAN_UNIT_ILLEG);

    if (dir_id_add(tmanfs.base, taskprj, taskid) != 0)
        return emod_set(TMAN_DIR_ID_MAKE);
    else if (unit_addbin(taskprj, taskid, units) != 0)
        return emod_set(TMAN_UNIT_MAKE);
    else if (task_add(taskprj, taskid))
        return emod_set(TMAN_COL_ADD);
    else if (opt->doswitch == TRUE && task_move(taskprj, taskid, COLCURR))
        return emod_set(TMAN_COL_MOVE);
    else if (hookact("add", taskprj, taskid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_show(tman_ctx_t *ctx, char *prj, char *id, struct tman_id_show_opt *opt)
{
    if ((status = chkargs(prj, id)))
        return status;

    /* Free task units because it might be called more than once.  */
    ctx->units.pgn = unit_delpgn(ctx->units.pgn);

    /* No need to check return value because there might case
     * that no hooks are defined or executed */
    ctx->units.pgn = hookshow(taskprj, taskid);
    if (unit_getbin(ctx->units.bin, taskprj, taskid) == NULL)
        status = emod_set(TMAN_UNIT_GET);

    strncpy(ctx->units.id, taskid, IDSIZ);
    return status;
}

int tman_id_col(tman_ctx_t *ctx, char *prj, char *id, char *tag, struct tman_id_col_opt *opt)
{
    // TODO: does not change CWD if curr task was moved.
    // The problem in CLI command `col'.

    if ((status = chkargs(prj, id)))
        return status;
    else if (col_ext(tag) == FALSE)
        return emod_set(TMAN_COL_EXISTS);
    return task_move(taskprj, taskid, tag);
}

int tman_id_del(tman_ctx_t *ctx, char *prj, char *id, struct tman_id_del_opt *opt)
{
    if ((status = chkargs(prj, id)))
        return status;

    /* FIXME: if current task gets deleted, plugin gun deletes
     * branches but won't switch to new current task branch.  */

    if (hookact("del", taskprj, taskid))
        return emod_set(TMAN_EHOOK);
    else if (unit_delbin(taskprj, taskid))
        return emod_set(TMAN_UNIT_DEL);
    else if (task_del(taskprj, taskid))
        return emod_set(TMAN_COL_DEL);
    else if (dir_id_del(tmanfs.base, taskprj, taskid))
        return emod_set(TMAN_DIR_ID_DEL);
    return TMAN_OK;
}

int tman_id_find_by_desc(tman_ctx_t *ctx, char *prj, char *descpatt)
{
    DIR *ids;
    struct dirent *ent;
    struct unit bunit[NKEYS];
    struct tree *node;
    char pgnout[PGNOUTSIZ + 1] = { 0 };
    char *desc;

    /* Free task ID list because it might be called more than once.  */
    tree_free(ctx->ids);
    ctx->ids = NULL;

    if ((status = check_input_prj(prj)))
        return status;
    else if ((ids = opendir(genpath_prj(taskprj))) == NULL)
        return emod_set(TMAN_DIR_PRJ_OPEN);

    while ((ent = readdir(ids)) != NULL) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if (descpatt == NULL)
            continue;
        else if (check_input_id(ent->d_name)) {
            // TODO: roach: sholud we leave it here? If not then what?..
            continue;
        }
        else if (unit_getbin(bunit, taskprj, ent->d_name) == NULL) {
            fprintf(stderr, "tman_id_list %s: failed to get units\n", ent->d_name);
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

        hookls(pgnout, taskprj, ent->d_name);
        struct column column = col_getmark(taskprj, ent->d_name);
        node = tree_alloc(ent->d_name, col_prio(column.col), desc, pgnout);
        ctx->ids = tree_add(ctx->ids, node);
        pgnout[0] = '\0';
    }
    closedir(ids);
    return TMAN_OK;
}

/*
 * roachme: refactor this shit.
 @param prj char * | NULL (then list the current project)
 @return struct item * | NULL (if error happened)
*/
int tman_id_list(tman_ctx_t *ctx, char *prj, struct tman_id_list_opt *opt)
{
    DIR *ids;
    struct dirent *ent;
    struct unit bunit[NKEYS];
    struct tree *node;
    char pgnout[PGNOUTSIZ + 1] = { 0 };

    /* Free task ID list because it might be called more than once.  */
    tree_free(ctx->ids);
    ctx->ids = NULL;

    if ((status = check_input_prj(prj)))
        return status;
    else if ((ids = opendir(genpath_prj(taskprj))) == NULL)
        return emod_set(TMAN_DIR_PRJ_OPEN);

    while ((ent = readdir(ids)) != NULL) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if (check_input_id(ent->d_name)) {
            // TODO: roach: sholud we leave it here? If not then what?..
            continue;
        }
        else if (unit_getbin(bunit, taskprj, ent->d_name) == NULL) {
            fprintf(stderr, "tman_id_list %s: failed to get units\n", ent->d_name);
            continue;
        }

        hookls(pgnout, taskprj, ent->d_name);
        struct column column = col_getmark(taskprj, ent->d_name);
        node = tree_alloc(ent->d_name, col_prio(column.col), bunit[3].val, pgnout);
        ctx->ids = tree_add(ctx->ids, node);
        pgnout[0] = '\0';
    }
    closedir(ids);
    return TMAN_OK;
}

/* Link task IDs together: parent - children relationship.  */
int tman_id_link(tman_ctx_t *ctx, char *prj, struct tman_id_list_opt *opt)
{
    return 0;
}

int tman_id_move(tman_ctx_t *ctx, char *srcprj, char *dstprj, char *srcid, char *dstid)
{
    char *_srcprj, *_srcid, *_dstprj, *_dstid;


    if ((status = chkargs(srcprj, srcid))) {
        return status;
    }
    _srcprj = taskprj;
    _srcid = taskid;

    if (dstid == NULL)
        dstid = srcid;
    if ((status = chkargs(dstprj, dstid)) && status != TMAN_ID_NOSUCH) {
        return status;
    }
    _dstprj = taskprj;
    _dstid = taskid;

    if (task_ext(_dstprj, _dstid) == TRUE) {
        return TMAN_ID_EXISTS;
    }

    /* TODO: add support to change user's PWD */

    /* Rename task. */
    if (strncmp(_srcprj, _dstprj, PRJSIZ) == 0) {
        if (dir_id_rename(tmanfs.base, _srcprj, _dstprj, _srcid, _dstid)) {
            return 1;
        }
    } else {
        /* Move task to another project. */
    }

    // roach: check this case
    // fprintf(stderr, "source and destination pathes are the same. Do nothing.");
    // TODO: update curr & prev if needed.
    return 0;
}

int tman_id_prev(tman_ctx_t *ctx, struct tman_id_prev_opt *opt)
{
    /* Check that current project and task ID are set.  */
    if ((status = chkargs(NULL, NULL)))
        return status;
    else if (task_prev(taskprj) == NULL)
        return emod_set(TMAN_ID_NOPREV);

    if (task_swap(taskprj))
        return emod_set(TMAN_ID_SWAP);
    else if (hookact("prev", prj_getcurr(), task_curr(taskprj)))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_set(tman_ctx_t *ctx, char *prj, char *id, struct unit *unitbin, struct tman_id_set_opt *opt)
{
    if ((status = chkargs(prj, id)))
        return status;
    else if (unit_chkbin(unitbin) == FALSE)
        return emod_set(TMAN_UNIT_ILLEG);

    if (unit_setbin(taskprj, taskid, unitbin))
        return emod_set(TMAN_UNIT_SET);
    else if (hookact("set", taskprj, taskid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

int tman_id_sync(tman_ctx_t *ctx, char *prj, char *id, struct tman_id_sync_opt *opt)
{
    if ((status = chkargs(prj, id)))
        return status;

    if (opt->doswitch == TRUE) {
        if (prj_iscurr(taskprj) == FALSE && prj_addcurr(taskprj) != 0)
            return emod_set(TMAN_PRJ_SWITCH);
        else if (task_iscurr(taskprj, taskid) == FALSE && task_move(taskprj, taskid, COLCURR))
            return emod_set(TMAN_ID_SWAP);
    }

    if (hookact("sync", taskprj, taskid))
        return emod_set(TMAN_EHOOK);
    return TMAN_OK;
}

char *tman_id_getcurr(tman_ctx_t *ctx, char *prj)
{
    if (chkargs(prj, NULL))
        return NULL;
    return strncpy(task_currid, taskid, IDSIZ);
}

char *tman_id_getprev(tman_ctx_t *ctx, char *prj)
{
    if (chkargs(prj, task_prev(prj)))
        return NULL;
    else if (task_prev(prj) == NULL)
        return NULL;
    return strncpy(task_previd, taskid, IDSIZ);
}

int tman_prj_add(tman_ctx_t *ctx, char *prj, struct tman_prj_add_opt *opt)
{
    /* Special case: project should not exists. If this's a case - let it go. */
    if ((status = check_input_prj(prj)) && status != TMAN_PRJ_NOSUCH)
        return status;
    else if (prj_exists(taskprj) == TRUE)
        return emod_set(TMAN_PRJ_EXISTS);
    else if (prj_chklen(taskprj) == FALSE)
        return emod_set(TMAN_PRJ_TOOLONG);

    if (dir_prj_add(tmanfs.base, taskprj))
        return emod_set(TMAN_DIR_PRJ_MAKE);
    else if (opt->doswitch == TRUE && prj_addcurr(taskprj))
        return emod_set(TMAN_PRJ_SWITCH);
    return TMAN_OK;
}

int tman_prj_del(tman_ctx_t *ctx, char *prj, struct tman_prj_del_opt *opt)
{
    if ((status = check_input_prj(prj)))
        return status;

    if (dir_prj_del(tmanfs.base, taskprj))
        return emod_set(TMAN_DIR_PRJ_DEL);
    else if (prj_iscurr(taskprj) == TRUE && prj_delcurr())
        return emod_set(TMAN_PRJ_DEL_CURR);
    else if (prj_isprev(taskprj) == TRUE && prj_delprev())
        return emod_set(TMAN_PRJ_DEL_CURR);
    return TMAN_OK;
}

/*
 * roachme: Refactor this shit
*/
int tman_prj_list(tman_ctx_t *ctx, struct tman_prj_list_opt *opt)
{
    DIR *edir;
    struct dirent *ent;
    struct tree *node;
    int colprio = 1;
    char *cprj, *pprj;
    char *desc = "some project desc";
    char pgnout[PGNOUTSIZ + 1] = { 0 };

    if ((edir = opendir(tmanfs.base)) == NULL)
        return emod_set(TMAN_DIR_PRJ_OPEN);

    /* Free task prj list because it might be called more than once.  */
    tree_free(ctx->prjs);
    ctx->prjs = NULL;

    cprj = prj_getcurr();
    pprj = prj_getprev();
    while ((ent = readdir(edir)) != NULL) {
        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if (check_input_prj(ent->d_name)) {
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

int tman_prj_prev(tman_ctx_t *ctx, struct tman_prj_prev_opt *opt)
{
    char *prjprev;

    if ((prjprev = prj_getprev()) == NULL)
        return emod_set(TMAN_PRJ_NOPREV);
    else if ((status = check_input_prj(NULL)))
        return status;
    else if ((status = check_input_prj(prjprev)))
        return status;

    if (prj_swapprjs())
        return emod_set(TMAN_PRJ_SWAP);
    return TMAN_OK;
}

int tman_prj_set(tman_ctx_t *ctx, char *prj, struct tman_prj_set_opt *opt)
{
    return TMAN_OK;
}

int tman_prj_sync(tman_ctx_t *ctx, char *prj, struct tman_prj_sync_opt *opt)
{
    if ((status = check_input_prj(prj)))
        return status;

    if (opt->doswitch == TRUE) {
        if (prj_iscurr(taskprj) == FALSE && prj_addcurr(taskprj) != 0)
            return emod_set(TMAN_PRJ_SWITCH);
    }
    return TMAN_OK;
}

char *tman_prj_getcurr(tman_ctx_t *ctx)
{
    if ((taskprj = prj_getcurr()) == NULL)
        return NULL;
    return strncpy(task_currprj, taskprj, PRJSIZ);
}

char *tman_prj_getprev(tman_ctx_t *ctx)
{
    if ((taskprj = prj_getprev()) == NULL)
        return NULL;
    return strncpy(task_prevprj, taskprj, PRJSIZ);
}

int tman_ispgn(const char *pgn)
{
    return ispgn(pgn);
}

int tman_pgnexec(tman_ctx_t *ctx, char *prj, char *id, char *pgname, char *pgncmd, struct tman_pgn_opt *opt)
{
    if ((status = chkargs(prj, id)))
        return status;
    return system(genpath_pgn(taskprj, taskid, pgname, pgncmd));
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
