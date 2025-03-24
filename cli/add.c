#include <stdio.h>
#include <stdlib.h>

#include "add.h"
#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_add(int argc, char **argv, struct tman_context *ctx)
{
    char *errfmt;
    struct tman_args args;
    int quiet, showhelp, status, i, c;
    struct tman_option opt = {
        .id_switch = TRUE,
        .id_generate = FALSE,
        .prj_switch = FALSE,
    };

    quiet = showhelp = FALSE;
    args.id = args.prj = NULL;
    errfmt = "cannot create task '%s': %s";
    while ((c = getopt(argc, argv, ":p:ghnq")) != -1) {
        switch (c) {
        case 'p':
            args.prj = optarg;
            break;
        case 'g':
            opt.id_generate = TRUE;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            opt.id_switch = FALSE;
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
        return help_usage("add");
    else if (optind == argc) {
        // TODO: find a good return code
        return elog(1, errfmt, "NOID", "task ID required");
    }

    for (i = optind; i < argc; ++i) {
        args.id = argv[i];
        tman_get_args(&args);

        if ((status = tman_check_arg_prj(&args))) {
            elog(status, errfmt, args.prj ? args.prj : "NOCURR",
                 tman_strerror());
            continue;
        } else if ((status = tman_check_arg_id(&args))
                   && status != TMAN_ID_NOSUCH) {
            elog(status, errfmt, args.id ? args.id : "NOCURR", tman_strerror());
            continue;
        }

        if ((status = tman_id_add(ctx, &args, &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
        }
    }
    return opt.id_switch && status == TMAN_OK ? tman_pwd() : status;
}
