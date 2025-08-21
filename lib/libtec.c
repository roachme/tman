#include <stdio.h>
#include <dirent.h>

#include "dir.h"
#include "unit.h"
#include "list.h"
#include "path.h"
#include "valid.h"
#include "osdep.h"
#include "errmod.h"
#include "libtec.h"

struct tecstruct {
    char db[PATH_MAX + 1];      /* directory for tec metadata */
    char base[PATH_MAX + 1];    /* directory for all stuff above */
};

static int aux_unit_set(tec_unit_t * newunits, const char *fname)
{
    struct tec_unit *item;
    struct tec_unit *units;

    if ((units = unit_load(fname)) == NULL)
        return emod_set(LIBTEC_UNIT_LOAD);

    // TODO: check values in atomic way

    for (item = newunits; item; item = item->next)
        unit_set(units, item->key, item->val);
    if (unit_save(fname, units))
        return emod_set(LIBTEC_UNIT_SAVE);

    unit_free(units);
    return LIBTEC_OK;
}

static int aux_list_get(tec_ctx_t * ctx, const char *dirname)
{
    DIR *ids;
    tec_arg_t args;
    struct dirent *ent;

    if ((ids = opendir(dirname)) == NULL)
        return emod_set(LIBTEC_DIR_OPEN);

    while ((ent = readdir(ids)) != NULL) {
        int status = LIBTEC_OK;
        args.taskid = ent->d_name;

        if (ent->d_name[0] == '.' || ent->d_type != DT_DIR)
            continue;
        else if (valid_task_name(args.taskid) == false)
            status = LIBTEC_ARG_ILLEG;

        ctx->list = list_add(ctx->list, args.taskid, status);
    }

    closedir(ids);
    return LIBTEC_OK;
}

static int fill_sysvars(const char *taskdir, struct tecstruct *tecfs)
{
    sprintf(tecfs->base, "%s", taskdir);
    sprintf(tecfs->db, "%s/%s", taskdir, ".tec");
    return LIBTEC_OK;
}

int tec_project_exist(const char *taskdir, tec_arg_t * args)
{
    char *pathname = path_project_dir(taskdir, args);
    return ISDIR(pathname) == true ? LIBTEC_OK : LIBTEC_ARG_NOSUCH;
}

int tec_board_exist(const char *taskdir, tec_arg_t * args)
{
    char *pathname = path_board_dir(taskdir, args);
    return ISDIR(pathname) == true ? LIBTEC_OK : LIBTEC_ARG_NOSUCH;
}

int tec_task_exist(const char *taskdir, tec_arg_t * args)
{
    char *pathname = path_task_dir(taskdir, args);
    return ISDIR(pathname) == true ? LIBTEC_OK : LIBTEC_ARG_NOSUCH;
}

int tec_project_valid(const char *taskdir, tec_arg_t * args)
{
    if (valid_project_name(args->project) == false)
        return emod_set(LIBTEC_ARG_ILLEG);
    return LIBTEC_OK;
}

int tec_board_valid(const char *taskdir, tec_arg_t * args)
{
    if (valid_board_name(args->board) == false)
        return emod_set(LIBTEC_ARG_ILLEG);
    return LIBTEC_OK;
}

int tec_task_valid(const char *taskdir, tec_arg_t * args)
{
    if (valid_task_name(args->taskid) == false)
        return emod_set(LIBTEC_ARG_ILLEG);
    return LIBTEC_OK;
}

int tec_make_db(const char *taskdir)
{
    struct tecstruct tecfs;

    if (fill_sysvars(taskdir, &tecfs))
        return emod_set(LIBTEC_SYS_MALLOC);
    else if (MKDIR(tecfs.base))
        return emod_set(LIBTEC_DIR_MAKE);
    else if (MKDIR(tecfs.db))
        return emod_set(LIBTEC_DIR_MAKE);
    return LIBTEC_OK;
}

int tec_check_db(const char *taskdir)
{
    struct tecstruct tecfs;

    if (fill_sysvars(taskdir, &tecfs))
        return emod_set(LIBTEC_SYS_MALLOC);
    else if (!ISDIR(tecfs.db))
        return emod_set(LIBTEC_SYS_DB);
    return LIBTEC_OK;
}

int tec_task_add(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if (dir_task_add(taskdir, args) != 0)
        return emod_set(LIBTEC_DIR_MAKE);
    else if (unit_save(path_task_unit(taskdir, args), ctx->units))
        return emod_set(LIBTEC_UNIT_SAVE);
    else if (unit_save(path_task_column(taskdir, args), ctx->column))
        return emod_set(LIBTEC_UNIT_SAVE);
    return LIBTEC_OK;
}

int tec_task_del(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if (dir_task_del(taskdir, args))
        return emod_set(LIBTEC_DIR_DEL);
    return LIBTEC_OK;
}

int tec_task_get(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if ((ctx->units = unit_load(path_task_unit(taskdir, args))) == NULL)
        return emod_set(LIBTEC_UNIT_GET);
    return LIBTEC_OK;
}

int tec_task_list(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    return aux_list_get(ctx, path_board_dir(taskdir, args));
}

int tec_task_move(const char *taskdir, tec_arg_t * src, tec_arg_t * dst,
                  tec_ctx_t * ctx)
{
    if (dir_task_move(taskdir, src, dst))
        return emod_set(LIBTEC_DIR_MOVE);
    return LIBTEC_OK;
}

int tec_task_set(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    return aux_unit_set(ctx->units, path_task_unit(taskdir, args));
}

int tec_task_column_get(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if ((ctx->column = unit_load(path_task_column(taskdir, args))) == NULL)
        return LIBTEC_UNIT_GET;
    return LIBTEC_OK;
}

int tec_task_column_set(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    return aux_unit_set(ctx->column, path_task_column(taskdir, args));
}

int tec_board_add(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if (dir_board_add(taskdir, args))
        return emod_set(LIBTEC_DIR_MAKE);
    else if (unit_save(path_board_unit(taskdir, args), ctx->units))
        return emod_set(LIBTEC_UNIT_SAVE);
    else if (unit_save(path_board_column(taskdir, args), ctx->column))
        return emod_set(LIBTEC_UNIT_SAVE);
    return LIBTEC_OK;
}

int tec_board_del(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if (dir_board_del(taskdir, args))
        return emod_set(LIBTEC_DIR_DEL);
    return LIBTEC_OK;
}

int tec_board_get(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if ((ctx->units = unit_load(path_board_unit(taskdir, args))) == NULL)
        return emod_set(LIBTEC_UNIT_GET);
    return LIBTEC_OK;
}

int tec_board_list(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    return aux_list_get(ctx, path_project_dir(taskdir, args));
}

int tec_board_move(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    return 0;
}

int tec_board_set(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    return aux_unit_set(ctx->units, path_board_unit(taskdir, args));
}

int tec_board_column_set(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if (unit_save(path_board_column(taskdir, args), ctx->column))
        return emod_set(LIBTEC_UNIT_SAVE);
    return LIBTEC_OK;
}

int tec_project_add(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if (dir_project_add(taskdir, args))
        return emod_set(LIBTEC_DIR_MAKE);
    else if (unit_save(path_project_unit(taskdir, args), ctx->units))
        return emod_set(LIBTEC_UNIT_SAVE);
    else if (unit_save(path_project_column(taskdir, args), ctx->column))
        return emod_set(LIBTEC_UNIT_SAVE);
    return LIBTEC_OK;
}

int tec_project_del(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if (dir_project_del(taskdir, args))
        return emod_set(LIBTEC_DIR_DEL);
    return LIBTEC_OK;
}

int tec_project_get(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    if ((ctx->units = unit_load(path_project_unit(taskdir, args))) == NULL)
        return emod_set(LIBTEC_UNIT_LOAD);
    return LIBTEC_OK;
}

int tec_project_list(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    return aux_list_get(ctx, taskdir);
}

int tec_project_rename(const char *taskdir, tec_arg_t * src, tec_arg_t * dst,
                       tec_ctx_t * ctx)
{
    if (dir_project_rename(taskdir, src, dst))
        return emod_set(LIBTEC_DIR_RENAME);
    return LIBTEC_OK;
}

int tec_project_set(const char *taskdir, tec_arg_t * args, tec_ctx_t * ctx)
{
    return aux_unit_set(ctx->units, path_project_unit(taskdir, args));
}

int tec_project_column_set(const char *taskdir, tec_arg_t * args,
                           tec_ctx_t * ctx)
{
    if (unit_save(path_project_column(taskdir, args), ctx->column))
        return emod_set(LIBTEC_UNIT_SAVE);
    return LIBTEC_OK;
}

tec_unit_t *tec_unit_add(struct tec_unit *head, char *key, char *val)
{
    return unit_add(head, key, val);
}

int tec_unit_set(struct tec_unit *head, char *key, char *val)
{
    // TODO: this will never fail, cuz in case of error it returns head.
    return !unit_set(head, key, val) ? LIBTEC_OK : LIBTEC_UNIT_SET;
}

int tec_unit_save(const char *filename, tec_unit_t * units)
{
    return unit_save(filename, units);
}

tec_unit_t *tec_unit_parse(struct tec_unit *head, const char *str)
{
    return unit_parse(head, str);
}

char *tec_unit_key(tec_unit_t * head, char *key)
{
    return unit_get(head, key);
}

void *tec_unit_free(tec_unit_t * units)
{
    unit_free(units);
    return NULL;
}

void *tec_list_free(tec_list_t * list)
{
    list_free(list);
    return NULL;
}

char *tec_strerror(int errnum)
{
    return emod_strerror(errnum);
}
