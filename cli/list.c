#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

static const char *errfmt = "cannot list tasks '%s': %s";

/*
Column names:
    todo
    ...
    ...
    ...
    done

Options:
    -a      show alls (default)
    -t      show only toggles
    -c COL  show tasks on specific column

Note:
    Tasks should be sorted by columns.
*/

struct list_filter {
    int toggle;
    char *column;
};

static struct list_filter filter = {
    .toggle = FALSE,
    .column = NULL,
};

// TODO: tman list -aT - fails. is that a bug?
// roachme: refactor this shit
static int check_filters(int quiet)
{
    if (filter.toggle) {
        filter.column = NULL;
    } else if (filter.column) {
        filter.toggle = FALSE;
    }
    return 0;
}

// TODO: remove parameter 'quiet', return status, and let the caller to
static char *get_unit_desc(tman_ctx_t * ctx, tman_arg_t * args, int quiet)
{
    int status;
    char *desc;

    desc = NULL;
    if ((status = tman_task_get(ctx, args))) {
        if (quiet == FALSE)
            elog(status, "'%s': %s one", args->taskid, tman_strerror(status));
    } else if ((desc = tman_unit_key(ctx->units, "desc")) == NULL) {
        if (quiet == FALSE)
            elog(1, "'%s': %s", args->taskid, "description not found");
    }
    return desc;
}

static char *get_column_name(tman_ctx_t * ctx, tman_arg_t * args, int quiet)
{
    int status;
    char *colname;

    colname = NULL;
    if ((status = tman_task_column_get(ctx, args))) {
        if (quiet == FALSE)
            elog(status, "'%s': %s one", args->taskid, tman_strerror(status));
    } else if ((colname = tman_unit_key(ctx->column, "name")) == NULL) {
        if (quiet == FALSE)
            elog(1, "'%s': %s", args->taskid, "description not found");
    }
    return colname;
}

static void show_column(tman_ctx_t * ctx, tman_arg_t * args, tman_list_t * obj,
                        int quiet)
{
    if (obj != NULL) {
        char *desc;
        char *colname;
        args->taskid = obj->name;

        if ((desc = get_unit_desc(ctx, args, quiet)) == NULL)
            return;
        else if ((colname = get_column_name(ctx, args, quiet)) == NULL)
            return;
        LIST_TASK_UNITS(colname, obj->name, desc);
        ctx->units = tman_unit_free(ctx->units);
        ctx->column = tman_unit_free(ctx->column);
    }
}

static int show_toggles(tman_ctx_t * ctx, tman_arg_t * args)
{
    int status;
    tman_list_t obj;

    args->taskid = NULL;
    if ((status = toggle_task_get_curr(tmancfg->base.task, args)) == 0) {
        obj.next = NULL;
        obj.status = LIBTMAN_OK;
        obj.name = args->taskid;
        show_column(ctx, args, &obj, FALSE);
    }

    args->taskid = NULL;
    if ((status = toggle_task_get_prev(tmancfg->base.task, args)) == 0) {
        obj.next = NULL;
        obj.status = LIBTMAN_OK;
        obj.name = args->taskid;
        show_column(ctx, args, &obj, FALSE);
    }
    return status;
}

static void show_columns(tman_ctx_t * ctx, tman_arg_t * args,
                         tman_list_t * list, int quiet)
{
    tman_list_t *obj;

    for (obj = list; obj != NULL; obj = obj->next) {
        show_column(ctx, args, obj, quiet);
    }
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_list(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    tman_arg_t args;
    int i, quiet, show_headers, status;

    quiet = show_headers = FALSE;
    args.project = args.board = args.taskid = NULL;
    while ((c = getopt(argc, argv, ":b:c:hqvtH")) != -1) {
        switch (c) {
        case 'b':
            args.board = optarg;
            break;
        case 'c':
            filter.column = optarg;
            break;
        case 'h':
            return help_usage("list");
        case 'q':
            quiet = TRUE;
            break;
        case 'v':
            return elog(1, "option `-%c' under development", c);
        case 't':
            filter.toggle = TRUE;
            break;
        case 'H':
            show_headers = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (check_filters(quiet))
        return 1;

    i = optind;
    do {
        args.project = argv[i];

        if ((status = toggle_project_get_curr(tmancfg->base.task, &args))) {
            if (quiet == FALSE)
                elog(status, errfmt, "NOCURR", "no current project");
            continue;
        } else if ((status = tman_check_arg_prj(&args))) {
            if (quiet == FALSE)
                elog(status, errfmt, args.project, tman_strerror(status));
            continue;
        } else if ((status = toggle_board_get_curr(tmancfg->base.task, &args))) {
            if (quiet == FALSE)
                elog(status, errfmt, "NOCURR", "no current board");
            continue;
        } else if ((status = tman_check_arg_brd(&args))) {
            if (quiet == FALSE)
                elog(status, errfmt, args.board, tman_strerror(status));
            continue;
        }

        if ((status = tman_task_list(ctx, &args)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, args.project, tman_strerror(status));
            continue;
        }

        if (show_headers == TRUE)
            printf("Project: %s\n", args.project);

        // TODO: add hooks
        // TODO: optimize object traverse (traverse multiple times)
        // TODO: optimize data structure load (it uses too much malloc)

        if (filter.toggle) {
            show_toggles(ctx, &args);
        } else if (filter.column) {
            elog(1, "show only task on column '%s'", filter.column);
            show_columns(ctx, &args, ctx->list, quiet);
        } else {
            elog(1, "show all tasks on all columns");
            show_columns(ctx, &args, ctx->list, quiet);
        }

        ctx->list = tman_list_free(ctx->list);

        // HOTFIX: cuz I've no clue how to sync board feature into projects.
        args.board = NULL;
    } while (++i < argc);
    return status;
}
