#include "cli.h"

int tman_cli_prev(int argc, char **argv, tman_ctx_t * ctx)
{
    char c, *errfmt;
    tman_arg_t args;
    int quiet, showhelp, status;
    tman_opt_t opt;

    quiet = showhelp = FALSE;
    args.prj = args.brd = args.task = NULL;
    errfmt = "cannot switch: %s";
    while ((c = getopt(argc, argv, ":b:p:hq")) != -1) {
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

    if (showhelp == TRUE)
        return help_usage("prev");

    if ((status = tman_task_prev(ctx, &args, &opt)) != LIBTMAN_OK) {
        if (quiet == FALSE)
            elog(status, errfmt, tman_strerror());
        return status;
    } else if (hook_action(&args, "prev")) {
        if (quiet == FALSE)
            elog(1, errfmt, args.task, "failed to execute hooks");
        return 1;
    }
    return tman_pwd();
}
