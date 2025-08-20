#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

int tec_cli_prev(int argc, char **argv, tec_ctx_t * ctx)
{
    tec_arg_t args;
    char c, *errfmt;
    int quiet, showhelp, status;

    quiet = showhelp = false;
    args.project = args.board = args.taskid = NULL;
    errfmt = "cannot switch to previous task '%s': %s";
    while ((c = getopt(argc, argv, ":b:hp:q")) != -1) {
        switch (c) {
        case 'b':
            args.board = optarg;
            break;
        case 'h':
            showhelp = true;
            break;
        case 'p':
            args.project = optarg;
            break;
        case 'q':
            quiet = true;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp == true)
        return help_usage("prev");

    if ((status = toggle_project_get_curr(teccfg.base.task, &args))) {
        if (quiet == false)
            elog(status, errfmt, "NOCURR", "no current project");
        return status;
    } else if ((status = toggle_board_get_curr(teccfg.base.task, &args))) {
        if (quiet == false)
            elog(status, errfmt, "NOCURR", "no current board");
        return status;
    } else if ((status = toggle_task_get_prev(teccfg.base.task, &args))) {
        if (quiet == false)
            elog(status, errfmt, "NOPREV", "no previous task");
        return status;
    } else if ((status = toggle_task_get_curr(teccfg.base.task, &args))) {
        if (quiet == false)
            elog(status, errfmt, "NOCURR", "no current task");
        return status;
    }

    if (toggle_task_swap(teccfg.base.task, &args)) {
        if (quiet == false)
            elog(1, "could not update toggle");
        return 1;
    } else if (hook_action(&args, "sync")) {
        if (quiet == false)
            elog(1, errfmt, args.taskid, "failed to execute hooks");
        return status;
    }
    return status == LIBTEC_OK ? tec_pwd_task(&args) : status;
}
