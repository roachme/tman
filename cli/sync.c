#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_sync(int argc, char **argv, struct tman_context *ctx)
{
    char c, *errfmt;
    struct tman_args args;
    int i, quiet, showhelp, status;
    struct tman_option opt = {
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
            return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("sync");

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

        if ((status = tman_id_sync(ctx, &args, &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
        }
        if ((status = tman_hook_action(ctx, &args, "sync")) != TMAN_OK)
            elog(status, errfmt, args.id, tman_strerror());
    } while (++i < argc);

    return opt.id_switch && status == TMAN_OK ? tman_pwd() : status;
}
