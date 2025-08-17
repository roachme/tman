#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../cli.h"
#include "config.h"
#include "toggle.h"

/* TODO:
    Structure: tec PGN -i -b -p COMMAND [OPTION]... [APRS]
    1. Can't use getopt cuz there might be different options for plugins and
       their commads.
    2. Use for/while loop to circle through options and their arguments.
    3. Separate plugin options from plugin command options.
    4. Or maybe it's better to let the plugin to handle plugin options and the rest.
*/
int tec_cli_plugin(int argc, char **argv, tec_ctx_t * ctx)
{
    int i;
    char *pgn;
    char *option;
    tec_arg_t args;
    char pgnopts[BUFSIZ + 1] = { 0 };
    char pgnexec[BUFSIZ + 1] = { 0 };
    int status;

    i = 0;
    pgn = option = NULL;
    args.project = args.board = args.taskid = NULL;

    pgn = argv[i++];

    /* Parse plugin command options.  */
    for (; i < argc; ++i) {
        option = argv[i];

        if (strcmp(option, "-b") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            args.board = argv[++i];
        } else if (strcmp(option, "-i") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            args.taskid = argv[++i];
        } else if (strcmp(option, "-p") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            args.project = argv[++i];
        } else {
            strcat(pgnopts, option);
            strcat(pgnopts, " ");
        }
    }

    /* Add default inputs if any. Do NOT check anything. Plugins should do.  */
    if ((status = toggle_project_get_curr(teccfg.base.task, &args))) {
        return status;
    }
    if ((status = toggle_board_get_curr(teccfg.base.task, &args))) {
        return status;
    }
    if ((status = toggle_task_get_curr(teccfg.base.task, &args))) {
        return status;
    }

    strcat(pgnexec, teccfg.base.pgn);
    strcat(pgnexec, "/");
    strcat(pgnexec, pgn);
    strcat(pgnexec, "/");
    strcat(pgnexec, pgn);

    strcat(pgnexec, " -T ");
    strcat(pgnexec, teccfg.base.task);
    strcat(pgnexec, " -P ");
    strcat(pgnexec, teccfg.base.pgn);

    strcat(pgnexec, " ");
    strcat(pgnexec, pgnopts);

    if (args.project != NULL) {
        strcat(pgnexec, " -p ");
        strcat(pgnexec, args.project);
    }
    if (args.board != NULL) {
        strcat(pgnexec, " -b ");
        strcat(pgnexec, args.board);
    }
    if (args.taskid != NULL) {
        strcat(pgnexec, " -i ");
        strcat(pgnexec, args.taskid);
    }

    dlog(1, "pgnexec: %s", pgnexec);
    return system(pgnexec);
}
