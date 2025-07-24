#include <string.h>

#include "cli.h"
#include "aux/column.h"

static const char *errfmt = "cannot list tasks '%s': %s";

#define COLUMN_CURR_MARK        '*'
#define COLUMN_PREV_MARK        '^'
#define COLUMN_BLOG_MARK        '+'
#define COLUMN_DONE_MARK        '-'

/*

Default output:
    1. Toggles
    2. Custom tasks

-a:
    1. default values
    2. backlog tasks

-c: only specified column

-t: for toggles only

-A: the whole pack
    1. default values
    2. backlog tasks
    3. finished tasks

-D: for finished tasks only

-T: for TODO/backlog tasks only

*/
struct list_filter {
    int all;
    int almostall;
    int blog;                   // -T
    char *column;               // -c
    int done;                   // -D
    int toggle;                 // -t
};

static struct list_filter filter = {
    .all = FALSE,
    .almostall = TRUE,
    .column = NULL,
    .blog = FALSE,
    .done = FALSE,
    .toggle = FALSE,
};

static char col_get_mark(char *name)
{
    for (int i = 0; i < g_colname_size; ++i)
        if (strcmp(name, g_colname[i].tag) == 0)
            return g_colname[i].mark;
    return g_colname[0].mark;
}

// TODO: remove parameter 'quiet', return status, and let the caller to
// print error message if needed
static char *get_unit_desc(tman_ctx_t * ctx, tman_arg_t * args, int quiet)
{
    int status;
    char *desc;

    desc = NULL;
    if ((status = tman_task_get(ctx, args, NULL))) {
        if (quiet == FALSE)
            elog(1, "'%s': %s one", args->task, tman_strerror());
    } else if ((desc = tman_unit_get_key(ctx->unitbin, "desc")) == NULL) {
        if (quiet == FALSE)
            elog(1, "'%s': %s", args->task, "description not found");
    }
    return desc;
}

static void show_column(tman_ctx_t * ctx, tman_arg_t * args, struct mylist *obj,
                        int *quiet)
{
    char mark;
    char *desc;

    if (obj != NULL) {
        mark = col_get_mark(obj->colname);
        args->task = obj->id;

        if ((desc = get_unit_desc(ctx, args, *quiet)) == NULL)
            return;
        LIST_TASK_UNITS(mark, obj->id, desc);
        tman_unit_free(ctx, args, NULL);
    }
}

static void show_columns(tman_ctx_t * ctx, tman_arg_t * args,
                         struct mylist *list, int *quiet)
{
    struct mylist *obj;

    for (obj = list; obj != NULL; obj = obj->next)
        show_column(ctx, args, obj, quiet);
}

static int check_column_name(char *column, int quiet)
{
    int column_found = FALSE;

    if (strcmp(column, "curr") == 0) {
        if (quiet == FALSE)
            elog(1, "'%s': is builtin column", column);
        return 1;
    } else if (strcmp(column, "prev") == 0) {
        if (quiet == FALSE)
            elog(1, "'%s': is builtin column", column);
        return 1;
    } else if (strcmp(column, "blog") == 0) {
        if (quiet == FALSE)
            elog(1, "'%s': is builtin column", column);
        return 1;
    } else if (strcmp(column, "done") == 0) {
        if (quiet == FALSE)
            elog(1, "'%s': is builtin column", column);
        return 1;
    }
    for (int k = 0; k < g_colname_size; ++k) {
        if (strcmp(g_colname[k].tag, column) == 0) {
            column_found = TRUE;
            break;
        }
    }
    if (column_found == FALSE) {
        if (quiet == FALSE)
            elog(1, "'%s': no such column name", column);
        return 1;
    }
    return 0;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_list(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    tman_arg_t args;
    struct mylist *obj;
    int i, quiet, showhelp, show_headers, status;

    /* TODO: get rid of option `-a'.  */
    quiet = showhelp = show_headers = FALSE;
    args.prj = args.brd = args.task = NULL;
    while ((c = getopt(argc, argv, ":ab:c:hqvDHST")) != -1) {
        switch (c) {
        case 'a':
            filter.all = TRUE;
            break;
        case 'b':
            args.brd = optarg;
            break;
        case 'c':
            filter.column = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case 'v':
            return elog(1, "option `-v' under development");
        case 'D':
            filter.done = TRUE;
            break;
        case 'H':
            show_headers = TRUE;
            break;
        case 'S':
            filter.toggle = TRUE;
            break;
        case 'T':
            filter.blog = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }
    if (filter.all && filter.toggle)
        return elog(1, "options `-A' and `-S' cant be used together");
    else if (filter.all && filter.done)
        return elog(1, "options `-A' and `-D' cant be used together");
    else if (filter.all && filter.blog)
        return elog(1, "options `-A' and `-T' cant be used together");
    else if (filter.all && filter.column)
        return elog(1, "options `-A' and `-c' cant be used together");

    if (filter.blog) {
        filter.all = FALSE;
        filter.almostall = FALSE;
    } else if (filter.toggle) {
        filter.all = FALSE;
        filter.almostall = FALSE;
    } else if (filter.column) {
        filter.all = FALSE;
        filter.almostall = FALSE;
    } else if (filter.done) {
        filter.all = FALSE;
        filter.almostall = FALSE;
    }

    if (showhelp == 1)
        return help_usage("list");

    i = optind;
    do {
        args.prj = argv[i];

        if ((status = tman_task_list(ctx, &args, NULL)) != LIBTMAN_OK) {
            args.prj = args.prj ? args.prj : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.prj, tman_strerror());
            continue;
        }

        if (show_headers == TRUE)
            printf("Project: %s\n", args.prj);

        // TODO: add hooks
        // TODO: optimize object traverse (traverse multiple times)
        // TODO: optimize data structure load (it uses too much malloc)

        if (filter.column != NULL) {
            if (check_column_name(filter.column, quiet))
                continue;
            for (obj = ctx->ids->custom; obj != NULL; obj = obj->next) {
                if (strcmp(filter.column, obj->colname) != 0)
                    continue;
                show_column(ctx, &args, obj, &quiet);
            }
        }

        if (filter.all || filter.almostall || filter.toggle) {
            show_columns(ctx, &args, ctx->ids->curr, &quiet);
            show_columns(ctx, &args, ctx->ids->prev, &quiet);
        }
        if (filter.all || filter.blog) {
            show_columns(ctx, &args, ctx->ids->blog, &quiet);
        }
        if (filter.all || filter.almostall) {
            for (int i = 0; i < g_colname_size; ++i) {
                for (obj = ctx->ids->custom; obj != NULL; obj = obj->next) {
                    if (strcmp(g_colname[i].tag, obj->colname) != 0)
                        continue;
                    obj->shown = TRUE;
                    show_column(ctx, &args, obj, &quiet);
                }
            }
            for (obj = ctx->ids->custom; obj != NULL; obj = obj->next) {
                if (quiet == FALSE && obj->shown != TRUE) {
                    char *task = obj->id;
                    char *colname = obj->colname;
                    elog(1, "'%s': invalid column name '%s'", task, colname);
                }
            }
        }
        if (filter.all || filter.done) {
            show_columns(ctx, &args, ctx->ids->done, &quiet);
        }

        if (quiet == FALSE) {
            for (obj = ctx->ids->inval; obj != NULL; obj = obj->next) {
                if (quiet == FALSE && obj->shown != TRUE) {
                    status = obj->status;
                    const char *fmt = "'%s': %s";
                    elog(status, fmt, obj->id, tman_strerror_get(status));
                }
            }
        }
    } while (++i < argc);
    return status;
}
