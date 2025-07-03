#include "cli.h"
#include "config.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_sync(int argc, char **argv, tman_ctx_t * ctx)
{
    char c, *errfmt;
    tman_arg_t args;
    int i, quiet, showhelp, status;
    tman_opt_t opt = {
        .id_switch = TRUE,
    };

    quiet = showhelp = FALSE;
    args.id = args.prj = NULL;
    errfmt = "cannot sync '%s': %s";
    while ((c = getopt(argc, argv, ":hnp:q")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            opt.id_switch = FALSE;
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
    tman_pwd_unset();
    do {
        args.id = argv[i];

        if ((status = tman_task_sync(ctx, &args, &opt)) != LIBTMAN_OK) {
            args.id = args.id ? args.id : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
            continue;
        } else if (hook_action(&args, "sync")) {
            if (quiet == FALSE)
                elog(status, errfmt, args.id, "failed to execute hooks");
            continue;
        }
    } while (++i < argc);

    return opt.id_switch && status == LIBTMAN_OK ? tman_pwd() : status;
}
