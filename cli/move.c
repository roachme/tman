#include "cli.h"

int tman_cli_move(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    int i, showhelp, status;
    tman_arg_t src = {
        .prj = NULL,
        .brd = NULL,
        .task = NULL,
    };
    tman_arg_t dst = {
        .prj = NULL,
        .brd = NULL,
        .task = NULL,
    };

    showhelp = FALSE;
    // TODO: add support for boards
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
        src.task = argv[i];
        dst.task = argv[i + 1];
        if ((status = tman_task_move(ctx, &src, &dst))) {
            return elog(status, "could not (re)move '%s': %s", src.task,
                        tman_strerror());
        }
    } else {
        do {
            src.task = dst.task = argv[i];
            if ((status = tman_task_move(ctx, &src, &dst))) {
                elog(status, "could not move '%s': %s", src.task,
                     tman_strerror());
            }
        } while (++i < argc);
    }
    return 0;
}
