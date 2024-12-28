#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_sync(int argc, char **argv, tman_ctx_t *ctx)
{
    char c, *errfmt;
    int showhelp, status;
    struct tman_id_sync_opt opt;

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
        return help_usage("sync");
    if ((status = tman_id_sync(ctx, &opt)) != TMAN_OK)
        return elog(status, errfmt, tman_strerror());
    return tman_pwd();
}
