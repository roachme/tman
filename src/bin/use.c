#include "use.h"
#include "../core.h"
#include "../common.h"

int tman_use_usage(void)
{
    const char *cmd = "use";
    printf("Usage: %s %s [OPTION]... ID\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

int tman_use(int argc, char **argv)
{
    char c;
    int status;
    struct tman_use_opt opt = { .env = NULL, .help = 0, };

    while ((c = getopt(argc, argv, ":e:h")) != -1) {
        switch (c) {
            case 'e':
                opt.env = optarg; break ;
            case 'h':
                opt.help = 1; break ;
            case ':':
                return elog(TMAN_INVOPT, "option `-%c' requires an argument", optopt);
            default:
                return elog(TMAN_INVOPT, "invalid option `%c'", optopt);
        }
    }

    if (opt.help == 1)
        return tman_use_usage();
    else if (optind == argc)
        return elog(TMAN_USE_IDREQ, "task id required");

    if ((status = core_id_use(NULL, argv[optind], &opt)) != 0)
        return status;
    return core_pwd();
}
