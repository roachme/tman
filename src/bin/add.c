#include <stdio.h>
#include <stdlib.h>

#include "add.h"
#include "../core/core.h"
#include "../core/common.h"

int tman_add_usage(void)
{
    printf("show some help info: under development\n");
    return TMAN_NOPATH;
}

int tman_add(int argc, char **argv)
{
    int c;
    int status;
    struct tman_add_opt opt = { .env = NULL, .force = 0, .help = 0, };

    while ((c = getopt(argc, argv, ":e:fh")) != -1) {
        switch (c) {
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
        return tman_add_usage();
    else if (optind == argc)
        return elog(TMAN_ADD_IDREQ, "task id required");

    for (int i = optind; i < argc; ++i)
        status = core_id_add(argv[i], &opt);
    return status == TMAN_OK ? core_currdir() : status;
}
