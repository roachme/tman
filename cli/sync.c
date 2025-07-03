#include "cli.h"

int tman_cli_sync(int argc, char **argv, tman_ctx_t * ctx)
{
    char c, *errfmt;
    tman_arg_t args;
    int i, quiet, showhelp, status;
    tman_opt_t opt = {
        .task_switch = TRUE,
    };

    quiet = showhelp = FALSE;
    args.prj = args.brd = args.task = NULL;
    errfmt = "cannot sync '%s': %s";
    while ((c = getopt(argc, argv, ":b:hnp:q")) != -1) {
        switch (c) {
        case 'b':
            args.brd = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            opt.task_switch = FALSE;
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
        return help_usage("sync");

    i = optind;
    do {
        args.task = argv[i];

        if ((status = tman_task_sync(ctx, &args, &opt)) != LIBTMAN_OK) {
            args.task = args.task ? args.task : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.task, tman_strerror());
            continue;
        } else if (hook_action(&args, "sync")) {
            if (quiet == FALSE)
                elog(status, errfmt, args.task, "failed to execute hooks");
            continue;
        }
    } while (++i < argc);

    return opt.task_switch && status == LIBTMAN_OK ? tman_pwd() : status;
}
