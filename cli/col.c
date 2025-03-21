#include <stdio.h>

#include "col.h"
#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_col(int argc, char **argv, tman_ctx_t * ctx)
{
    char *prj, *col;
    int i, c, status, showhelp, showlist;
    struct tman_id_col_opt opt;

    prj = NULL;
    showhelp = showlist = FALSE;
    while ((c = getopt(argc, argv, ":hl")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'l':
            showlist = TRUE;
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

    for (i = optind; i < argc; ++i)
        if ((status = tman_id_col(ctx, prj, argv[i], col, &opt)) != TMAN_OK)
            elog(status, "cannot move to column '%s': %s", col,
                 tman_strerror());

    if (optind == argc)         /* operate on current task id */
        if ((status = tman_id_col(ctx, prj, argv[i], col, &opt)) != TMAN_OK)
            elog(status, "cannot move to column '%s': %s", col,
                 tman_strerror());

    return status;
}
