#include <stdio.h>

#include "col.h"
#include "cli.h"
#include "../src/tman.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_col(int argc, char **argv)
{
    int c;
    char *col;
    int status;
    int help = 0;
    int list = 0;
    char *newcid, *oldcid = column_getcid();

    while ((c = getopt(argc, argv, ":hl")) != -1) {
        switch (c) {
            case 'h':
                help = 1; break;
            case 'l':
                list = 1; break;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (help == 1)
        return help_usage("col");
    else if (list == 1) {
        printf("show list of columns\n");
        return 1;
    } else if ((col = argv[optind++]) == NULL)
        return elog(1, "gotta specify column to move a task to");

    for (int i = optind; i < argc; ++i)
        status = tman_id_col(NULL, argv[i], col);

    if (optind == argc) /* operate on current task id */
        status = tman_id_col(NULL, NULL, col);

    return status;
}