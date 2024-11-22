#include <stdio.h>

#include "col.h"
#include "../core.h"
#include "../help.h"
#include "../common.h"

int tman_col(int argc, char **argv)
{
    int c;
    int status;
    struct tman_mcol_opt opt = { .col = NULL, .env = NULL, .force = 0, .help = 0, };

    while ((c = getopt(argc, argv, ":c:e:fh")) != -1) {
        switch (c) {
            case 'c':
                opt.col = optarg; break;
            case 'e':
                opt.env = optarg; break;
            case 'f':
                opt.force = 1; break;
            case 'h':
                opt.help = 1; break;
            case ':':
                return elog(TMAN_INVOPT, "option `-%c' requires an argument", optopt);
            default:
                return elog(TMAN_INVOPT, "invalid option `%c'", optopt);
        }
    }

    if (opt.help == 1)
        return help_usage("add");
    else if (opt.col == NULL)
        return elog(1, "gotta specify column to move a task to");

    for (int i = optind; i < argc; ++i)
        status = core_id_movecol(opt.env, argv[i], opt.col);

    if (optind == argc) /* operate on current task id */
        status = core_id_movecol(opt.env, NULL, opt.col);
    return status;
}
