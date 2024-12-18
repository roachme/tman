#include "del.h"
#include "../src/tman.h"

int tman_cli_del_usage(void)
{
    const char *cmd = "del";
    printf("Usage: %s %s [OPTION]... [ID]..\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_del(int argc, char **argv)
{
    char c;
    int status;
    struct tman_cli_del_opt opt = { .env = NULL, .force = 0, .help = 0,};

    while ((c = getopt(argc, argv, ":e:fh")) != -1) {
        switch (c) {
            case 'e':
                opt.env = optarg; break ;
            case 'f':
                opt.force = 1; break ;
            case 'h':
                opt.help = 1; break ;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (opt.help == 1)
        return tman_cli_del_usage();

    for (int i = optind; i < argc; ++i)
        status = tman_id_del(opt.env, argv[i], &opt);

    if (optind == argc) /* delete current task id */
        status = tman_id_del(opt.env, NULL, &opt);

    // FIXME: when delete task ID from non-current env,
    // it switches to current task in current env.
    // BUT should not change user's CWD at all.
    return status == TMAN_OK ? tman_pwd() : status;
}