#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

int tman_cli_prev(int argc, char **argv, tman_ctx_t * ctx)
{
    tman_arg_t args;
    char c, *errfmt;
    int quiet, showhelp, status;

    quiet = showhelp = FALSE;
    args.project = args.board = args.taskid = NULL;
    errfmt = "cannot switch to previous task '%s': %s";
    while ((c = getopt(argc, argv, ":b:hp:q")) != -1) {
        switch (c) {
        case 'b':
            args.board = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'p':
            args.project = optarg;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("prev");

    if ((status = toggle_project_get_curr(tmancfg.base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current project");
        return status;
    } else if ((status = toggle_board_get_curr(tmancfg.base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current board");
        return status;
    } else if ((status = toggle_task_get_prev(tmancfg.base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOPREV", "no previous task");
        return status;
    } else if ((status = toggle_task_get_curr(tmancfg.base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current task");
        return status;
    }

    if (toggle_task_swap(tmancfg.base.task, &args)) {
        if (quiet == FALSE)
            elog(1, "could not update toggle");
        return 1;
    } else if (hook_action(&args, "sync")) {
        if (quiet == FALSE)
            elog(1, errfmt, args.taskid, "failed to execute hooks");
        return status;
    }
    return status == LIBTMAN_OK ? tman_pwd_task(&args) : status;
}
