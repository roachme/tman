#include "cli.h"
#include "plugin.h"

int tman_cli_plugin(char *name, int argc, char **argv, tman_ctx_t *ctx)
{
    char c, *cmd, *env, *id, *subcmd;

    id = env = subcmd = NULL;
    while ((c = getopt(argc, argv, ":e:i:")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break;
            case 'i':
                id = optarg; break;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (optind == argc)
        cmd = "";
    else
        cmd = argv[optind];

    return tman_plugin(NULL, env, id, name, cmd, NULL);
}
