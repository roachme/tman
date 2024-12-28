#include <stdio.h>
#include <stdlib.h>

#include "add.h"
#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_add(int argc, char **argv, tman_ctx_t *ctx)
{
    char *env, *errfmt;
    int quiet, force, showhelp, status, i, c;
    struct tman_id_add_opt opt = {
        .noswitch = FALSE,
    };

    env = NULL;
    force = quiet = showhelp = FALSE;
    errfmt = "cannot create task '%s': %s";
    while ((c = getopt(argc, argv, ":e:fhnq")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break;
            case 'f':
                force = TRUE; break;
            case 'h':
                showhelp = TRUE; break;
            case 'n':
                opt.noswitch = TRUE; break;
            case 'q':
                quiet = TRUE; break;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("add");
    else if (optind == argc) {
        // TODO: find a good return code
        return elog(1, errfmt, "NOID", "task ID required");
    }

    for (i = optind; i < argc; ++i) {
        if ((status = tman_id_add(ctx, env, argv[i], &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
            if (force == FALSE)
                break;
        }
    }
    return status == TMAN_OK ? tman_pwd() : status;
}
