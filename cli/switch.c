#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

int tman_cli_switch(int argc, char **argv, tman_ctx_t * ctx)
{
    tman_arg_t args;
    char c, *errfmt, *colname;
    int i, quiet, showhelp, status, update_toggle, switch_dir;

    colname = "todo";           /* Default column to move task to is 'todo'.  */
    errfmt = "cannot sync '%s': %s";
    update_toggle = TRUE;
    quiet = showhelp = switch_dir = FALSE;
    args.prj = args.brd = args.task = NULL;
    while ((c = getopt(argc, argv, ":b:c:hnp:qN")) != -1) {
        switch (c) {
        case 'b':
            args.brd = optarg;
            break;
        case 'c':
            colname = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            update_toggle = FALSE;
            break;
        case 'p':
            args.prj = optarg;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case 'N':
            switch_dir = FALSE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("sync");

    if ((status = toggle_prj_get_curr(tmancfg->base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current project");
        return status;
    } else if ((status = toggle_brd_get_curr(tmancfg->base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current board");
        return status;
    } else if (column_exist(colname) == FALSE) {
        if (quiet == FALSE)
            elog(1, "'%s': no such column", colname);
        return 1;
    } else if ((ctx->column = generate_column(colname)) == NULL) {
        if (quiet == FALSE)
            elog(1, "could not generate column");
        return 1;
    }
    // TODO: prevent column update when new column name is the same as old
    i = optind;
    do {
        args.task = argv[i];

        if ((status = toggle_task_get_curr(tmancfg->base.task, &args))) {
            if (quiet == FALSE)
                elog(status, errfmt, "NOCURR", "no current task");
            return status;
        } else if ((status = tman_task_column_set(ctx, &args)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, args.task, tman_strerror());
            continue;
        } else if (hook_action(&args, "sync")) {
            if (quiet == FALSE)
                elog(status, errfmt, args.task, "failed to execute hooks");
            continue;
        }
    } while (++i < argc);

    if ((update_toggle && status == LIBTMAN_OK)
        && toggle_task_set_curr(tmancfg->base.task, &args)) {
        if (quiet == FALSE)
            elog(status, "could not update toggles");
        return 1;
    }
    return (switch_dir && status == LIBTMAN_OK) ? tman_pwd_task(&args) : status;
}
