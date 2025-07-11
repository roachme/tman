#include <stdio.h>

#include "cli.h"

/*
TODO: add commands like
    next     - move to next column in workflow
    prev     - move to previous column in workflow
    show     - show columns
*/

static int show_columns(char *prj)
{
    // TODO: add support for project columns
    struct column {
        char mark;
        char tag[10];
        char desc[80];
    } columns[] = {
        {.tag = "curr",.mark = '*',.desc = "current task"},
        {.tag = "prev",.mark = '^',.desc = "previous task"},
        {.tag = "blog",.mark = '+',.desc = "backlog column"},

// user define columns (gotta read 'em from config file
        /*
           {.tag = "revw",.mark = '>',.desc = "review column"},
           {.tag = "test",.mark = '$',.desc = "test column"},
           {.tag = "lock",.mark = '!',.desc = "locked column"},
           {.tag = "done",.mark = '-',.desc = "done column"},
         */
    };

    for (int i = 0; i < sizeof(columns) / sizeof(columns[0]); ++i)
        printf("%c %s \t\t%s\n", columns[i].mark, columns[i].tag,
               columns[i].desc);
    return 0;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_flow(int argc, char **argv, tman_ctx_t * ctx)
{
    tman_arg_t args;
    int i, c, status, showhelp, showlist;
    tman_opt_t opt;
    const char *errfmt = "cannot move to column '%s' to '%s': %s";

    args.prj = args.brd = args.task = NULL;
    showhelp = showlist = FALSE;
    // TODO: add an option to specify project
    while ((c = getopt(argc, argv, ":b:hlp:")) != -1) {
        switch (c) {
        case 'b':
            args.brd = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'l':
            showlist = TRUE;
            break;
        case 'p':
            args.prj = optarg;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp == 1)
        return help_usage("flow");
    else if (showlist == 1) {
        return show_columns("");
    } else if ((ctx->colname = argv[optind++]) == NULL)
        return elog(1, "gotta specify column to move a task to");

    // TODO: does not change CWD if curr task was moved.
    // The problem in CLI command `col'.
    i = optind;
    do {
        args.task = argv[i];
        if ((status = tman_task_flow(ctx, &args, &opt)) != LIBTMAN_OK)
            elog(status, errfmt, args.task, ctx->colname, tman_strerror());
    } while (++i < argc);

    return status;
}
