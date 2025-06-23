#include <stdio.h>
#include <stdlib.h>

#include "cli.h"
#include "config.h"

#define xstr(s)     str(s)
#define str(s)      #s
#define IDLIMIT     99999999

static char genid[IDSIZ + 1];

static int generate_id(struct tman_arg *args)
{
    for (unsigned int i = 1; i < IDLIMIT; ++i) {
        sprintf(genid, "%0" xstr(IDSIZ) "u", i);
        args->id = genid;
        if (tman_check_arg_id_exist(args) == FALSE)
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
    args.id = args.brd = args.prj = NULL;
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
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("add");
    else if ((status = tman_check_arg_prj(&args))) {
        args.prj = args.prj ? args.prj : "NOCURR";
        if (quiet == FALSE)
            elog(status, errfmt, args.prj, tman_strerror());
        return status;
    } else if (optind == argc && generate_id(&args)) {
        if (quiet == FALSE)
            elog(1, "could not generate task ID");
        return 1;
    }

    tman_pwd_unset();
    i = optind;
    do {
        args.id = args.id == NULL ? argv[i] : args.id;

        if ((status = tman_task_add(ctx, &args, &opt)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
            args.id = NULL;     /* unset task ID, not to break loop.  */
            continue;
        } else if (tmancfg->usehooks == TRUE
                   && (status =
                       tman_hook_action(ctx, tmancfg->hooks, &args,
                                        "add")) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(LIBTMAN_EHOOK, errfmt, args.id, tman_strerror());
            args.id = NULL;     /* unset task ID, not to break loop.  */
            continue;
        }
        /* TODO: find a better trick.  */
        args.id = NULL;         /* unset task ID, not to break loop.  */
    } while (++i < argc);
    return opt.id_switch && status == LIBTMAN_OK ? tman_pwd() : status;
}
