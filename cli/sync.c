#include <string.h>

#include "cli.h"

static int is_builtin_column(tman_ctx_t * ctx)
{
    tman_column_t colnames[] = {
        {.name = "blog",.prio = 0,},
        {.name = "curr",.prio = 1,},
        {.name = "prev",.prio = 2,},
        {.name = "done",.prio = 3,},
    };

    for (int i = 0; i < sizeof(colnames) / sizeof(colnames[0]); ++i)
        if (strcmp(ctx->colname, colnames[i].name) == 0) {
            ctx->colprio = colnames[i].prio;
            return TRUE;
        }
    return FALSE;
}

int tman_cli_sync(int argc, char **argv, tman_ctx_t * ctx)
{
    tman_arg_t args;
    char c, *errfmt;
    int i, quiet, showhelp, status, switch_dir;
    tman_opt_t opt = {
        .task_switch = TRUE,
    };

    switch_dir = TRUE;
    ctx->colname = "curr";
    quiet = showhelp = FALSE;
    args.prj = args.brd = args.task = NULL;
    errfmt = "cannot sync '%s': %s";
    while ((c = getopt(argc, argv, ":b:c:hnp:qN")) != -1) {
        switch (c) {
        case 'b':
            args.brd = optarg;
            break;
        case 'c':
            ctx->colname = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            opt.task_switch = FALSE;
            break;
        case 'p':
            args.prj = optarg;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case 'N':
            switch_dir = FALSE;
            opt.task_switch = FALSE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("sync");
    else if (!is_builtin_column(ctx))
        return elog(1, "'%s': no such builtin column", ctx->colname);

    i = optind;
    do {
        args.task = argv[i];

        /* Note: can't move to previous column directly.  */
        if (ctx->colprio == 2)
            args.task = NULL;

        if ((status = tman_task_sync(ctx, &args, &opt)) != LIBTMAN_OK) {
            args.task = args.task ? args.task : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.task, tman_strerror());
            continue;
        } else if (hook_action(&args, "sync")) {
            if (quiet == FALSE)
                elog(status, errfmt, args.task, "failed to execute hooks");
            continue;
        }
    } while (++i < argc);

    // TODO: switch_dir only when it's needed.

    return switch_dir && status == LIBTMAN_OK ? tman_pwd_task(&args) : status;
}
