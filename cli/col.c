#include <stdio.h>

#include "col.h"
#include "cli.h"
#include "../src/tman.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_col(int argc, char **argv)
{
    char *env, *col;
    int i, c, status, showhelp, showlist;

    env = NULL;
    showhelp = showlist = FALSE;
    while ((c = getopt(argc, argv, ":hl")) != -1) {
        switch (c) {
            case 'h':
                showhelp = TRUE; break;
            case 'l':
                showlist = TRUE; break;
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
        status = tman_id_col(env, argv[i], col);

    if (optind == argc) /* operate on current task id */
        status = tman_id_col(env, NULL, col);

    return status;
}
