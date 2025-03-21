#include "cli.h"
#include "plugin.h"

int tman_cli_plugin(char *name, int argc, char **argv, tman_ctx_t * ctx)
{
    int status;
    char c, *cmd, *prj, *id;

    id = prj = NULL;
    while ((c = getopt(argc, argv, ":p:i:")) != -1) {
        switch (c) {
        case 'p':
            prj = optarg;
            break;
        case 'i':
            id = optarg;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `%c'", optopt);
        }
    }

    /* roachme: system call gotta take care of it.  */
    if (optind == argc)
        cmd = "";
    else
        cmd = argv[optind];

    if ((status = tman_pgnexec(NULL, prj, id, name, cmd, NULL)) != TMAN_OK)
        elog(status, "pgn failed: %s", tman_strerror());
    return status;
}
