#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

int tec_cli_sync(int argc, char **argv, tec_ctx_t * ctx)
{
    tec_arg_t args;
    char c, *errfmt;
    int i, quiet, showhelp, status;
    int switch_toggle, switch_dir;

    quiet = showhelp = false;
    switch_toggle = switch_dir = true;
    errfmt = "cannot sync (with) '%s': %s";
    args.project = args.board = args.taskid = NULL;
    while ((c = getopt(argc, argv, ":b:hnp:qN")) != -1) {
        switch (c) {
        case 'b':
            args.board = optarg;
            break;
        case 'h':
            showhelp = true;
            break;
        case 'n':
            switch_toggle = false;
            break;
        case 'p':
            args.project = optarg;
            break;
        case 'q':
            quiet = true;
            break;
        case 'N':
            switch_dir = false;
            switch_toggle = false;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp == true)
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
            if (quiet == false)
                elog(status, errfmt, args.taskid, "failed to execute hooks");
            continue;
        }
    } while (++i < argc);

    if (status == LIBTEC_OK && switch_toggle == true) {
        if (toggle_task_set_curr(teccfg.base.task, &args) && quiet == false)
            status = elog(1, "could not update toggles");
    }

    return status == LIBTEC_OK && switch_dir ? tec_pwd_task(&args) : status;
}
