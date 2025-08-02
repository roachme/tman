#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

int tman_cli_sync(int argc, char **argv, tman_ctx_t * ctx)
{
    tman_arg_t args;
    char c, *errfmt;
    int i, quiet, showhelp, status;
    int switch_toggle, switch_dir;

    quiet = showhelp = FALSE;
    switch_toggle = switch_dir = TRUE;
    errfmt = "cannot sync (with) '%s': %s";
    args.project = args.board = args.taskid = NULL;
    while ((c = getopt(argc, argv, ":b:hnp:qN")) != -1) {
        switch (c) {
        case 'b':
            args.board = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            switch_toggle = FALSE;
            break;
        case 'p':
            args.project = optarg;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case 'N':
            switch_dir = FALSE;
            switch_toggle = FALSE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("sync");

    if ((status = toggle_project_get_curr(tmancfg->base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current project");
        return status;
    } else if ((status = toggle_board_get_curr(tmancfg->base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current board");
        return status;
    }

    i = optind;
    do {
        args.taskid = argv[i];

        if ((status = toggle_task_get_curr(tmancfg->base.task, &args))) {
            if (quiet == FALSE)
                elog(status, errfmt, "NOCURR", "no current task");
            continue;
        } else if ((status = tman_check_arg_task(&args))) {
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, tman_strerror(status));
            continue;
        } else if (hook_action(&args, "sync")) {
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, "failed to execute hooks");
            continue;
        }
    } while (++i < argc);

    if (status == LIBTMAN_OK && switch_toggle == TRUE) {
        if (toggle_task_set_curr(tmancfg->base.task, &args) && quiet == FALSE)
            status = elog(1, "could not update toggles");
    }

    return status == LIBTMAN_OK && switch_dir ? tman_pwd_task(&args) : status;
}
