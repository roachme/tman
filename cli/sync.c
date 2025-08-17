#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

int tec_cli_sync(int argc, char **argv, tec_ctx_t * ctx)
{
    tec_arg_t args;
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

    if ((status = check_arg_project(&args, errfmt, quiet)))
        return status;
    else if ((status = check_arg_board(&args, errfmt, quiet)))
        return status;

    i = optind;
    do {
        args.taskid = argv[i];

        if ((status = check_arg_task(&args, errfmt, quiet)))
            continue;
        else if (hook_action(&args, "sync")) {
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, "failed to execute hooks");
            continue;
        }
    } while (++i < argc);

    if (status == LIBTEC_OK && switch_toggle == TRUE) {
        if (toggle_task_set_curr(teccfg.base.task, &args) && quiet == FALSE)
            status = elog(1, "could not update toggles");
    }

    return status == LIBTEC_OK && switch_dir ? tec_pwd_task(&args) : status;
}
