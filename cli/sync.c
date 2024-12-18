#include "../src/tman.h"

static int tman_cli_sync_usage(void)
{
    const char *cmd = "sync";
    printf("Usage: %s %s [OPTION]\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_sync(int argc, char **argv)
{
    char c;
    int help = 0;
    int status;

    while ((c = getopt(argc, argv, ":h")) != -1) {
        switch (c) {
            case 'h':
                help = 1; break;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (help == 1)
        return tman_cli_sync_usage();
    return (status = tman_id_sync()) == TMAN_OK ? tman_pwd() : status;
}
