#include <stdio.h>
#include <stdlib.h>

#include "add.h"
#include "cli.h"
#include "../src/tman.h"

static int tman_cli_add_usage(void)
{
    const char *cmd = "add";
    printf("Usage: %s %s [OPTION]... [ID]..\n", PROGRAM, cmd);
    printf("Try '%s help %s' for more info.\n", PROGRAM, cmd);
    return TMAN_OK;
}

int tman_cli_add(int argc, char **argv)
{
    int c;
    int status;
    struct tman_cli_add_opt opt = {
        .env = NULL,
        .force = FALSE,
        .help = FALSE,
        .noswitch = FALSE
    };

    while ((c = getopt(argc, argv, ":e:fhn")) != -1) {
        switch (c) {
            case 'e':
                opt.env = optarg; break;
            case 'f':
                opt.force = TRUE; break;
            case 'h':
                opt.help = TRUE; break;
            case 'n':
                opt.noswitch = TRUE; break;
            case ':':
                return elog(TMAN_INVOPT, "option `-%c' requires an argument", optopt);
            default:
                return elog(TMAN_INVOPT, "invalid option `%c'", optopt);
        }
    }

    if (opt.help == TRUE)
        return help_usage("add");
    else if (optind == argc)
        return elog(TMAN_ADD_IDREQ, "task id required");

    for (int i = optind; i < argc; ++i)
        status = tman_id_add(opt.env, argv[i], &opt);
    return status == TMAN_OK ? tman_pwd() : status;
}
