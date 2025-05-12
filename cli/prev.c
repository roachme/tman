#include "cli.h"
#include "config.h"

int tman_cli_prev(int argc, char **argv, struct tman_context *ctx)
{
    char c, *errfmt;
    struct tman_arg args;
    int quiet, showhelp, status;
    struct tman_option opt;

    quiet = showhelp = FALSE;
    args.id = args.prj = NULL;
    errfmt = "cannot switch: %s";
    while ((c = getopt(argc, argv, ":hq")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
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
        return help_usage("prev");

    if ((status = tman_task_prev(ctx, &args, &opt)) != TMAN_OK) {
        if (quiet == FALSE)
            elog(status, errfmt, tman_strerror());
        return status;
    } else if ((status =
                tman_hook_action(ctx, tman_config->hooks, &args,
                                 "prev")) != TMAN_OK) {
        if (quiet == FALSE)
            elog(status, errfmt, tman_strerror());
        return status;
    }

    return status == TMAN_OK ? tman_pwd() : status;
}
