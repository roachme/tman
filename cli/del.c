#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_del(int argc, char **argv, tman_ctx_t * ctx)
{
    tman_arg_t args;
    char c, *errfmt;
    tman_opt_t opt = { };
    int i, choice, quiet, showhelp, showprompt, status;

    showprompt = TRUE;
    quiet = showhelp = FALSE;
    args.prj = args.brd = args.task = NULL;
    errfmt = "cannot delete task '%s': %s";
    while ((c = getopt(argc, argv, ":b:hnp:q")) != -1) {
        switch (c) {
        case 'b':
            args.brd = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            showprompt = FALSE;
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

    if (showhelp == TRUE)
        return help_usage("del");
    // TODO: if not current task gets deleted, then no need to
    // change user's current directory.
    i = optind;
    do {
        args.task = argv[i];

        /* Get and check input values explicitly because it's one of the rare
         * cases when hooks get exectude before the main action.  */
        if ((status = tman_check_arg_prj(&args))) {
            args.prj = args.prj ? args.prj : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.prj, tman_strerror());
            continue;
        } else if ((status = tman_check_arg_brd(&args))) {
            args.brd = args.brd ? args.brd : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.brd, tman_strerror());
            continue;
        } else if ((status = tman_check_arg_task(&args))) {
            args.task = args.task ? args.task : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.task, tman_strerror());
            continue;
        }

        if (showprompt) {
            printf("Are you sure to delete task '%s'? [y/N] ", args.task);
            choice = getchar();
            if (choice != 'y' && choice != 'Y')
                continue;
        }

        if (hook_action(&args, "del")) {
            if (quiet == FALSE)
                elog(1, errfmt, args.task, "failed to execute hooks");
            continue;
        }
        if ((status = tman_task_del(ctx, &args, &opt)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, args.task, tman_strerror());
            continue;
        }
    } while (++i < argc);

    // FIXME: when delete task ID from non-current prj,
    // it switches to current task in current prj.
    // BUT should not change user's CWD at all.
    return status == LIBTMAN_OK ? tman_pwd() : status;
}
