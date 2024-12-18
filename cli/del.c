#include "del.h"
#include "../src/tman.h"

int tman_cli_del_usage(void)
{
    const char *cmd = "del";
    printf("Usage: %s %s [OPTION]... [ID]..\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_del(int argc, char **argv)
{
    char c, *env, *errfmt;
    struct tman_id_del_opt opt = { };
    int i, force, quiet, showhelp, status;

    env =  NULL;
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
        return tman_cli_del_usage();

    for (i = optind; i < argc; ++i) {
        if ((status = tman_id_del(env, argv[i], &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_get_errmsg());
            if (force == TRUE)
                break;
        }
    }

    if (optind == argc) { /* delete current task id */
        if ((status = tman_id_del(env, argv[i], &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_get_errmsg());
        }
    }

    // FIXME: when delete task ID from non-current env,
    // it switches to current task in current env.
    // BUT should not change user's CWD at all.
    return status == TMAN_OK ? tman_pwd() : status;
}
