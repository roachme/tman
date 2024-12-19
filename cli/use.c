#include "use.h"
#include "cli.h"

int tman_cli_use_usage(void)
{
    const char *cmd = "use";
    printf("Usage: %s %s [OPTION]... ID\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_use(int argc, char **argv, tman_ctx_t *ctx)
{
    char c, *errfmt, *env;
    int showhelp, status;
    struct tman_id_use_opt opt;

    env = NULL;
    showhelp = FALSE;
    errfmt = "cannot switch to '%s': %s";
    while ((c = getopt(argc, argv, ":e:h")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break ;
            case 'h':
                showhelp = 1; break ;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp == 1)
        return tman_cli_use_usage();
    else if (optind == argc)
        return elog(TMAN_USE_IDREQ, "task id required");

    if ((status = tman_id_use(ctx, env, argv[optind], &opt)) != TMAN_OK) {
        return elog(status, errfmt, argv[optind], tman_strerror());
    }
    return tman_pwd();
}
