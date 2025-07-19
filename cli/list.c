#include "cli.h"

static const char *errfmt = "cannot list tasks '%s': %s";

struct list_filter {
    int all;
    int blog;
    int toggle;
};

static struct list_filter filter = {
    .all = FALSE,
    .blog = FALSE,
    .toggle = TRUE,
};

// TODO: remove parameter 'quiet', return status, and let the caller to
static char *get_unit_desc(tman_ctx_t * ctx, tman_arg_t * args, int quiet)
{
    int status;
    char *desc;

    desc = NULL;
    if ((status = tman_task_get(ctx, args, NULL))) {
        if (quiet == FALSE)
            elog(status, "'%s': %s one", args->task, tman_strerror());
    } else if ((desc = tman_unit_get_key(ctx->unitbin, "desc")) == NULL) {
        if (quiet == FALSE)
            elog(1, "'%s': %s", args->task, "description not found");
    }
    return desc;
}

static void show_column(tman_ctx_t * ctx, tman_arg_t * args, struct mylist *obj,
                        int *quiet)
{
    if (obj != NULL) {
        char *desc;
        args->task = obj->id;

        if ((desc = get_unit_desc(ctx, args, *quiet)) == NULL)
            return;
        LIST_TASK_UNITS(obj->colname, obj->id, desc);
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

// TODO: Find a good error message in case option fails.  */
int tman_cli_list(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    tman_arg_t args;
    struct mylist *obj;
    int i, quiet, show_headers, status;

    quiet = show_headers = FALSE;
    args.prj = args.brd = args.task = NULL;
    while ((c = getopt(argc, argv, ":ab:hqvHST")) != -1) {
        switch (c) {
        case 'a':
            filter.all = TRUE;
            break;
        case 'b':
            args.brd = optarg;
            break;
        case 'h':
            return help_usage("list");
        case 'q':
            quiet = TRUE;
            break;
        case 'v':
            return elog(1, "option `-%c' under development", c);
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

    if (filter.blog) {
        filter.all = FALSE;
        filter.toggle = FALSE;
    }

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

        if (filter.all || filter.toggle) {
            show_columns(ctx, &args, ctx->ids->curr, &quiet);
            show_columns(ctx, &args, ctx->ids->prev, &quiet);
        }
        if (filter.all || filter.blog) {
            show_columns(ctx, &args, ctx->ids->blog, &quiet);
        }
        if (quiet == FALSE) {
            for (obj = ctx->ids->inval; obj != NULL; obj = obj->next) {
                const char *fmt = "'%s': %s";
                elog(status, fmt, obj->id, tman_strerror_get(obj->status));
            }
        }
    } while (++i < argc);
    return status;
}
