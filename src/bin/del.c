#include "del.h"
#include "../core/common.h"
#include "../core/core.h"

int tman_del_usage(void)
{
    printf("show some help info: under development\n");
    return TMAN_NOPATH;
}

int tman_del(int argc, char **argv)
{
    char c;
    int status;
    struct tman_del_opt opt = { .env = NULL, .force = 0, .help = 0,};

    while ((c = getopt(argc, argv, ":e:fh")) != -1) {
        switch (c) {
            case 'e':
                opt.env = optarg; break ;
            case 'f':
                opt.force = 1; break ;
            case 'h':
                opt.help = 1; break ;
            case ':':
                return elog(TMAN_INVOPT, "option `-%c' requires an argument", optopt);
            default:
                return elog(TMAN_INVOPT, "invalid option `%c'", optopt);
        }
    }

    if (opt.help == 1)
        return tman_del_usage();

    for (int i = optind; i < argc; ++i)
        status = core_id_del(argv[i], &opt);

    if (optind == argc) /* delete current task id */
        status = core_id_del(opt.env, NULL);

    // FIXME: when delete task ID from non-current env,
    // it switches to current task in current env.
    // BUT should not change user's CWD at all.
    return status == TMAN_OK ? core_currdir() : status;
}
