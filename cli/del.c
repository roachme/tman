#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

// TODO: Find a good error message in case option fails.  */
int tec_cli_del(int argc, char **argv, tec_ctx_t * ctx)
{
    tec_arg_t args;
    char c, *errfmt;
    int i, choice, quiet, showhelp, autoconfirm, status;

    autoconfirm = quiet = showhelp = FALSE;
    args.project = args.board = args.taskid = NULL;
    errfmt = "cannot delete task '%s': %s";
    while ((c = getopt(argc, argv, ":b:hp:qy")) != -1) {
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
        case 'y':
            autoconfirm = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("del");

    if ((status = check_arg_project(&args, errfmt, quiet)))
        return status;
    else if ((status = check_arg_board(&args, errfmt, quiet)))
        return status;

    // TODO: if non-current task gets deleted, then no need to
    // change user's current directory.
    i = optind;
    do {
        args.taskid = argv[i];

        /* Get and check input values explicitly because it's one of the rare
         * cases when hooks get exectude before the main action.  */
        if ((status = check_arg_task(&args, errfmt, quiet))) {
            continue;
        } else if (autoconfirm == FALSE) {
            printf("Are you sure to delete task '%s'? [y/N] ", args.taskid);
            if ((choice = getchar()) != 'y' && choice != 'Y')
                continue;
        }

        if (hook_action(&args, "del")) {
            if (quiet == FALSE)
                elog(1, errfmt, args.taskid, "failed to execute hooks");
            continue;
        } else if ((status = tec_task_del(teccfg.base.task, &args, ctx))) {
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, tec_strerror(status));
            continue;
        }
    } while (++i < argc);

    // FIXME: when delete task ID from non-current project,
    // it switches to current task in current project.
    // BUT should not change user's CWD at all.
    return status == LIBTEC_OK ? tec_pwd_task(&args) : status;
}
