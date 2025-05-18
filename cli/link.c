#include <stdio.h>
#include "cli.h"
#include "config.h"
#include "help.h"

/*
 * TODO: add support to show child/parent links
 * TODO: add support to add multiple child/parent links
 */
int tman_cli_link(int argc, char **argv, struct tman_context *ctx)
{
    int c, status;
    struct tman_arg parent, child;
    int quiet, showhelp, do_unlink;

    quiet = showhelp = do_unlink = FALSE;
    child.id = child.brd = child.prj = NULL;
    parent.id = parent.brd = parent.prj = NULL;

    while ((c = getopt(argc, argv, ":hrp:q")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'r':
            do_unlink = TRUE;
            return elog(1, "option `-%c' under development", c);
            break;
        case 'p':
            child.prj = parent.prj = optarg;
            break;
        case 'q':
            quiet = TRUE;
            break;
        }
    }

    if (showhelp)
        return help_usage("link");
    else if ((argc - optind) != 2) {
        elog(1, "invalid number of arguments: child or parent task ID missing");
        return help_usage("link");
    }

    parent.id = argv[optind];
    child.id = argv[optind + 1];

    if ((status = tman_task_link(ctx, &parent, &child, NULL)) != LIBTMAN_OK) {
        if (quiet == FALSE)
            elog(status, "could not link tasks '%s' and '%s': %s", parent.id,
                 child.id, tman_strerror());
        return status;
    }
    return LIBTMAN_OK;
}
