#include "del.h"
#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_del(int argc, char **argv, tman_ctx_t *ctx)
{
    char c, *env, *id, *errfmt;
    struct tman_id_del_opt opt = { };
    int i, force, quiet, showhelp, status;

    env = id =  NULL;
    force = quiet = showhelp = FALSE;
    errfmt = "cannot delete task '%s': %s";
    while ((c = getopt(argc, argv, ":e:fhq")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break ;
            case 'f':
                force = TRUE; break ;
            case 'h':
                showhelp = TRUE; break ;
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

    i = optind;
    do {
        if ((status = tman_id_del(ctx, env, argv[i], &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
            if (force == TRUE)
                break;
        }
    } while (++i < argc);

    // FIXME: when delete task ID from non-current env,
    // it switches to current task in current env.
    // BUT should not change user's CWD at all.
    return status == TMAN_OK ? tman_pwd() : status;
}
