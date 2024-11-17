#include "../core.h"
#include "../common.h"

static int tman_sync_usage(void)
{
    const char *cmd = "sync";
    printf("Usage: %s %s [OPTION]\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_NOPATH;
}

int tman_sync(int argc, char **argv)
{
    char c;
    int help = 0;
    int status;

    while ((c = getopt(argc, argv, ":h")) != -1) {
        switch (c) {
            case 'h':
                help = 1; break;
            case ':':
                return elog(TMAN_INVOPT, "option `-%c' requires an argument", optopt);
            default:
                return elog(TMAN_INVOPT, "invalid option `%c'", optopt);
        }
    }

    if (help == 1)
        return tman_sync_usage();
    return (status = core_id_sync()) == TMAN_OK ? core_currdir() : status;
}
