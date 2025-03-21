#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_sync(int argc, char **argv, tman_ctx_t * ctx)
{
    char c, *errfmt, *prj;
    int i, quiet, showhelp, status;
    struct tman_id_sync_opt opt = {
        .doswitch = TRUE,
    };

    prj = NULL;
    quiet = showhelp = FALSE;
    errfmt = "cannot sync '%s': %s";
    while ((c = getopt(argc, argv, ":hnp:q")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            opt.doswitch = FALSE;
            break;
        case 'p':
            prj = optarg;
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
        if ((status = tman_id_sync(ctx, prj, argv[i], &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
        }
    } while (++i < argc);

    return opt.doswitch && status == TMAN_OK ? tman_pwd() : status;
}
