#include "cli.h"
#include "aux/config.h"

/*

Usage: tec move SRC DST

SRC: ././task1 ././task11   -> rename task1 to task11 in current project and current board
SRC: task1 task11           -> rename task1 to task11 in current project and current board

Notes:
    '.'     - current project/board/task
    '..'    - previous project/board/task

'.' in arguments can be omited so use current arg by default.
*/

int tec_cli_move(int argc, char **argv, tec_ctx_t * ctx)
{
    char c;
    int i, showhelp, status;
    tec_arg_t dst, src;

    showhelp = FALSE;
    src.project = src.board = src.taskid = NULL;
    dst.project = dst.board = dst.taskid = NULL;
    // TODO: add support for boards
    while ((c = getopt(argc, argv, ":d:hs:")) != -1) {
        switch (c) {
        case 'd':
            dst.project = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 's':
            src.project = optarg;
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
        src.taskid = argv[i];
        dst.taskid = argv[i + 1];
        if ((status = tec_task_move(teccfg.base.task, &src, &dst, ctx))) {
            return elog(status, "could not (re)move '%s': %s", src.taskid,
                        tec_strerror(status));
        }
    } else {
        do {
            src.taskid = dst.taskid = argv[i];
            if ((status = tec_task_move(teccfg.base.task, &src, &dst, ctx))) {
                elog(status, "could not move '%s': %s", src.taskid,
                     tec_strerror(status));
            }
        } while (++i < argc);
    }
    return 0;
}
