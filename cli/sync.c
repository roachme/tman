#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

int tman_cli_sync(int argc, char **argv, tman_ctx_t * ctx)
{
    char *errfmt;
    tman_arg_t args;
    int c, i, quiet, status, showhelp;

    quiet = showhelp = FALSE;
    errfmt = "'%s': could not switch to current task";
    args.prj = args.brd = args.task = NULL;
    while ((c = getopt(argc, argv, ":b:hp:q")) != -1) {
        switch (c) {
        case 'b':
            args.brd = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'p':
            args.prj = optarg;
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

    if ((status = toggle_prj_get_curr(tmancfg->base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current project");
    } else if ((status = tman_check_arg_prj(&args))) {
        if (quiet == FALSE)
            elog(status, errfmt, args.prj, tman_strerror());
    } else if ((status = toggle_brd_get_curr(tmancfg->base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current board");
    } else if ((status = tman_check_arg_brd(&args))) {
        if (quiet == FALSE)
            elog(status, errfmt, args.brd, tman_strerror());
    } else if ((status = toggle_task_get_curr(tmancfg->base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current task");
    } else if ((status = tman_check_arg_task(&args))) {
        if (quiet == FALSE)
            elog(status, errfmt, args.brd, tman_strerror());
    }
    return status == LIBTMAN_OK ? tman_pwd_task(&args) : status;
}
