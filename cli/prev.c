#include "prev.h"
#include "cli.h"

static int tman_cli_prev_usage(void)
{
    const char *cmd = "sync";
    printf("Usage: %s %s [OPTION]\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

int tman_cli_prev(int argc, char **argv, tman_ctx_t *ctx)
{
    char c, *errfmt;
    int showhelp, status;
    struct tman_id_prev_opt opt;

    showhelp = FALSE;
    errfmt =  "cannot switch: %s";
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
        return tman_cli_prev_usage();
    if ((status = tman_id_prev(ctx, &opt)) != TMAN_OK)
        return elog(1, errfmt, tman_strerror());
    return tman_pwd();
}
