#include <stdio.h>

#include "col.h"
#include "cli.h"

static int show_columns(char *prj)
{
    // TODO: add support for project columns
    struct column {
        char mark;
        char tag[10];
        char desc[80];
    } columns[] = {
        {.tag = "uknw",.mark = '?',.desc = "unknown tag"},
        {.tag = "curr",.mark = '*',.desc = "current task"},
        {.tag = "prev",.mark = '^',.desc = "previous task"},
        {.tag = "blog",.mark = '+',.desc = "backlog column"},

// user define columns (gotta read 'em from config file
        {.tag = "revw",.mark = '>',.desc = "review column"},
        {.tag = "test",.mark = '$',.desc = "test column"},
        {.tag = "lock",.mark = '!',.desc = "locked column"},
        {.tag = "done",.mark = '-',.desc = "done column"},
    };

    for (int i = 0; i < sizeof(columns) / sizeof(columns[0]); ++i)
        printf("%c %s \t\t%s\n", columns[i].mark, columns[i].tag,
               columns[i].desc);
    return 0;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_col(int argc, char **argv, struct tman_context *ctx)
{
    char *col;
    struct tman_arg args;
    int i, c, status, showhelp, showlist;
    struct tman_option opt;
    const char *errfmt = "cannot move to column '%s': %s";

    args.prj = args.id = NULL;
    showhelp = showlist = FALSE;
    // TODO: add an option to specify project
    while ((c = getopt(argc, argv, ":hlp:")) != -1) {
        switch (c) {
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
            return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp == 1)
        return help_usage("col");
    else if (showlist == 1) {
        return show_columns("");
    } else if ((col = argv[optind++]) == NULL)
        return elog(1, "gotta specify column to move a task to");

    // TODO: does not change CWD if curr task was moved.
    // The problem in CLI command `col'.
    i = optind;
    do {
        args.id = argv[i];
        tman_get_args(&args);

        if ((status = tman_check_arg_prj(&args)))
            return elog(status, errfmt, col, tman_strerror());
        if ((status = tman_id_col(ctx, &args, col, &opt)) != TMAN_OK)
            elog(status, errfmt, col, tman_strerror());
    } while (++i < argc);

    return status;
}
