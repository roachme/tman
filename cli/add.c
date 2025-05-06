#include <stdio.h>
#include <stdlib.h>

#include "add.h"
#include "cli.h"
#include "config.h"

#define xstr(s)     str(s)
#define str(s)      #s
#define IDLIMIT     99999999

static char genid[IDSIZ + 1];

static int generate_id(struct tman_arg *args)
{
    for (unsigned int i = 1; i < IDLIMIT; ++i) {
        sprintf(genid, "%0" xstr(IDSIZ) "d", i);
        args->id = genid;
        if (tman_check_arg_id(args))
            return 0;
    }
    return 1;
}

int tman_cli_add(int argc, char **argv, struct tman_context *ctx)
{
    char *errfmt;
    struct tman_arg args;
    int quiet, showhelp, status, i, c;
    struct tman_option opt = {
        .id_switch = TRUE,
        .id_generate = FALSE,
        .prj_switch = FALSE,
    };

    quiet = showhelp = FALSE;
    args.id = args.prj = NULL;
    errfmt = "cannot create task '%s': %s";
    while ((c = getopt(argc, argv, ":p:hnq")) != -1) {
        switch (c) {
        case 'p':
            args.prj = optarg;
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

    tman_get_args(&args);
    args.id = NULL;             /* unset current task ID */

    if (showhelp == TRUE)
        return help_usage("add");
    else if (optind == argc && generate_id(&args))
        return elog(1, "could not generate task ID");

    i = optind;
    do {
        args.id = args.id == NULL ? argv[i] : args.id;
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

        if ((status = tman_task_add(ctx, &args, &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
        } else if ((status =
                    tman_hook_action(ctx, tman_config->hooks, &args,
                                     "add")) != TMAN_OK)
            if (quiet == FALSE)
                elog(TMAN_EHOOK, errfmt, args.id, tman_strerror());
        args.id = NULL;
    } while (++i < argc);
    return opt.id_switch && status == TMAN_OK ? tman_pwd() : status;
}
