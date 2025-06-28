#include "cli.h"
#include "help.h"

int tman_cli_move(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    int i, showhelp, status;
    tman_arg_t src = {
        .id = NULL,
        .brd = NULL,
        .prj = NULL,
    };
    tman_arg_t dst = {
        .id = NULL,
        .brd = NULL,
        .prj = NULL,
    };

    showhelp = FALSE;
    while ((c = getopt(argc, argv, ":d:hs:")) != -1) {
        switch (c) {
        case 'd':
            dst.prj = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 's':
            src.prj = optarg;
            break;
        }
    }

    if (showhelp)
        return help_usage("move");

    i = optind;

    // some checks
    if ((argc - i) == 1) {
        elog(1, "destination task is missing");
        return 1;
    }

    if ((argc - i) == 2) {
        src.id = argv[i];
        dst.id = argv[i + 1];
        if ((status = tman_task_move(ctx, &src, &dst))) {
            return elog(status, "could not (re)move '%s': %s", src.id,
                        tman_strerror());
        }
    } else {
        do {
            src.id = dst.id = argv[i];
            if ((status = tman_task_move(ctx, &src, &dst))) {
                elog(status, "could not move '%s': %s", src.id,
                     tman_strerror());
            }
        } while (++i < argc);
    }
    return 0;
}
