#include "cli.h"

static int tman_cli_sync_usage(void)
{
    const char *cmd = "sync";
    printf("Usage: %s %s [OPTION]\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_sync(int argc, char **argv, struct tman_context *ctx)
{
    char c, *errfmt;
    int showhelp, status;

    showhelp = FALSE;
    errfmt =  "cannot sync: %s";
    while ((c = getopt(argc, argv, ":h")) != -1) {
        switch (c) {
            case 'h':
                showhelp = TRUE; break;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return tman_cli_sync_usage();
    if ((status = tman_id_sync()) != TMAN_OK)
        return elog(status, errfmt, tman_strerror());
    return tman_pwd();
}
