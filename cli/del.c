#include "del.h"
#include "cli.h"
#include "config.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_del(int argc, char **argv, struct tman_context *ctx)
{
    struct tman_arg args;
    char c, *errfmt;
    struct tman_option opt = { };
    int i, choice, quiet, showhelp, showprompt, status;

    showprompt = TRUE;
    quiet = showhelp = FALSE;
    args.id = args.prj = NULL;
    errfmt = "cannot delete task '%s': %s";
    while ((c = getopt(argc, argv, ":fhnp:q")) != -1) {
        switch (c) {
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
            return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("del");
    else if (showprompt) {
        /* Hotfix: if you add to stdout, than it'll be catched in my shell.  */
        fprintf(stderr, "Are you sure to delete task(s)? [y/N] ");
        choice = getchar();
        if (choice != 'y' && choice != 'Y') {
            return 0;
        }
    }
    // TODO: if not current task gets deleted, then no need to
    // change user's current directory.
    i = optind;
    do {
        args.id = argv[i];
        tman_get_args(&args);

        if ((status = tman_check_arg_prj(&args))) {
            elog(status, errfmt, args.prj ? args.prj : "NOCURR",
                 tman_strerror());
            continue;
        } else if ((status = tman_check_arg_id(&args))) {
            elog(status, errfmt, args.id ? args.id : "NOCURR", tman_strerror());
            continue;
        }

        if ((status =
             tman_hook_action(ctx, tman_config->hooks, &args,
                              "del")) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
            continue;
        }
        if ((status = tman_task_del(ctx, &args, &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
        }
    } while (++i < argc);

    // FIXME: when delete task ID from non-current prj,
    // it switches to current task in current prj.
    // BUT should not change user's CWD at all.
    return status == TMAN_OK ? tman_pwd() : status;
}
