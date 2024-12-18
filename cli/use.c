#include "use.h"
#include "../src/tman.h"

int tman_cli_use_usage(void)
{
    const char *cmd = "use";
    printf("Usage: %s %s [OPTION]... ID\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_use(int argc, char **argv)
{
    char c;
    int status;
    struct tman_cli_use_opt opt = { .env = NULL, .help = 0, };

    while ((c = getopt(argc, argv, ":e:h")) != -1) {
        switch (c) {
            case 'e':
                opt.env = optarg; break ;
            case 'h':
                opt.help = 1; break ;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (opt.help == 1)
        return tman_cli_use_usage();
    else if (optind == argc)
        return elog(TMAN_USE_IDREQ, "task id required");

    if ((status = tman_id_use(NULL, argv[optind], &opt)) != TMAN_OK) {
        return elog(status, "cannot switch to '%s': %s", argv[optind], tman_get_errmsg());
    }
    return tman_pwd();
}
