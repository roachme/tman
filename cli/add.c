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

// TODO: Find a good error message in case option fails.  */
int tman_cli_add(int argc, char **argv)
{
    int i, c;
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
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (opt.help == TRUE)
        return help_usage("add");
    else if (optind == argc) {
        // TODO: find a good return code
        return elog(1, "task id required");
    }

    for (i = optind; i < argc; ++i) {
        if ((status = tman_id_add(opt.env, argv[i], &opt)) != TMAN_OK) {
            elog(status, "cannot create '%s': %s", argv[i], tman_get_errmsg());
        }
    }
    return status == TMAN_OK ? tman_pwd() : status;
}
