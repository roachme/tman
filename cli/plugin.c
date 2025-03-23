#include "cli.h"
#include "plugin.h"

int tman_cli_plugin(char *name, int argc, char **argv, struct tman_context *ctx)
{
    int status;
    char c, *cmd;
    struct tman_args args;

    args.id = args.prj = NULL;
    while ((c = getopt(argc, argv, ":p:i:")) != -1) {
        switch (c) {
        case 'p':
            args.prj = optarg;
            break;
        case 'i':
            args.id = optarg;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `%c'", optopt);
        }
    }
    cmd = optind == argc ? "" : argv[optind];
    tman_get_args(&args);

    if ((status = tman_pgnexec(NULL, &args, name, cmd, NULL)) != TMAN_OK)
        elog(status, "pgn failed: %s", tman_strerror());
    return status;
}
