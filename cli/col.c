#include <stdio.h>

#include "col.h"
#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_col(int argc, char **argv, struct tman_context *ctx)
{
    char *col;
    struct tman_args args;
    int i, c, status, showhelp, showlist;
    struct tman_id_col_opt opt;
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
        printf("show list of columns\n");
        return TMAN_OK;
    } else if ((col = argv[optind++]) == NULL)
        return elog(1, "gotta specify column to move a task to");

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
