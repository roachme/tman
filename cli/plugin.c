#include "cli.h"

/* TODO:
    Structure: tman PGN -i -b -p COMMAND [OPTION]... [APRS]
    1. Can't use getopt cuz there might be different options for plugins and
       their commads.
    2. Use for/while loop to circle through options and their arguments.
    3. Separate plugin options from plugin command options.
    4. Or maybe it's better to let the plugin to handle plugin options and the rest.
*/
int tman_cli_plugin(int argc, char **argv, struct tman_context *ctx)
{
    int status;
    char c, *cmd, *name;
    struct tman_arg args;

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

    name = argv[optind++];
    cmd = optind == argc ? "" : argv[optind++];
    if ((status = tman_pgnexec(NULL, &args, name, cmd, NULL)) != LIBTMAN_OK)
        elog(status, "pgn failed: %s", tman_strerror());
    return status;
}
