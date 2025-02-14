#include "del.h"
#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_del(int argc, char **argv, tman_ctx_t *ctx)
{
    char c, *prj, *id, *errfmt;
    struct tman_id_del_opt opt = { };
    int i, choice, force, quiet, showhelp, showprompt, status;

    prj = id =  NULL;
    showprompt = TRUE;
    force = quiet = showhelp = FALSE;
    errfmt = "cannot delete task '%s': %s";
    while ((c = getopt(argc, argv, ":fhnp:q")) != -1) {
        switch (c) {
            case 'f':
                force = TRUE; break ;
            case 'h':
                showhelp = TRUE; break ;
            case 'n':
                showprompt = FALSE; break ;
            case 'p':
                prj = optarg; break ;
            case 'q':
                quiet = TRUE; break ;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("del");
    else if (showprompt) {
        /* Hotfix: if you add to stdout, than it'll be catched in my shell.  */
        fprintf(stderr, "Are you sure to delete task(s)? [y/N] ");
        choice = getchar();
        if (choice != 'y' && choice != 'Y') {
            return 0;
        }
    }

    i = optind;
    do {
        if ((status = tman_id_del(ctx, prj, argv[i], &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
            if (force == TRUE)
                break;
        }
    } while (++i < argc);

    // FIXME: when delete task ID from non-current prj,
    // it switches to current task in current prj.
    // BUT should not change user's CWD at all.
    return status == TMAN_OK ? tman_pwd() : status;
}
