#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_del(int argc, char **argv, tman_ctx_t * ctx)
{
    tman_arg_t args;
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

    if ((status = toggle_project_get_curr(tmancfg->base.task, &args))) {
        return status;
    }
    if ((status = toggle_board_get_curr(tmancfg->base.task, &args))) {
        return status;
    }
    // TODO: if non-current task gets deleted, then no need to
    // change user's current directory.
    i = optind;
    do {
        args.taskid = argv[i];

        /* Get and check input values explicitly because it's one of the rare
         * cases when hooks get exectude before the main action.  */
        if ((status = toggle_task_get_curr(tmancfg->base.task, &args))) {
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
        } else if ((status = tman_task_del(ctx, &args)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, tman_strerror(status));
            continue;
        }
    } while (++i < argc);

    // FIXME: when delete task ID from non-current prj,
    // it switches to current task in current prj.
    // BUT should not change user's CWD at all.
    return status == LIBTMAN_OK ? tman_pwd_task(&args) : status;
}
