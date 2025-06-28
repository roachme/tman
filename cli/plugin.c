#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cli.h"
#include "config.h"

/* TODO:
    Structure: tman PGN -i -b -p COMMAND [OPTION]... [APRS]
    1. Can't use getopt cuz there might be different options for plugins and
       their commads.
    2. Use for/while loop to circle through options and their arguments.
    3. Separate plugin options from plugin command options.
    4. Or maybe it's better to let the plugin to handle plugin options and the rest.
*/
int tman_cli_plugin(int argc, char **argv, tman_ctx_t * ctx)
{
    int i;
    char *pgn;
    char *option;
    tman_arg_t args;
    char pgnopts[BUFSIZ + 1] = { 0 };
    char pgnexec[BUFSIZ + 1] = { 0 };
    int status;

    i = 0;
    pgn = option = NULL;
    args.brd = args.id = args.prj = NULL;

    pgn = argv[i++];

    tman_pwd_unset();

    /* Parse plugin command options.  */
    for (; i < argc; ++i) {
        option = argv[i];

        if (strcmp(option, "-b") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            args.brd = argv[++i];
        } else if (strcmp(option, "-i") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            args.id = argv[++i];
        } else if (strcmp(option, "-p") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            args.prj = argv[++i];
        } else {
            strcat(pgnopts, option);
            strcat(pgnopts, " ");
        }
    }

    /* Check system options.  */
    if ((status = tman_check_arg_prj(&args)) && status != LIBTMAN_PRJ_NOCURR)
        return elog(status, "invalid project: %s", tman_strerror());
    else if ((status = tman_check_arg_brd(&args))
             && status != LIBTMAN_BRD_NOCURR)
        return elog(1, "invalid board: %s", tman_strerror());
    else if ((status = tman_check_arg_id(&args)) && status != LIBTMAN_ID_NOCURR)
        return elog(1, "invalid task ID: %s", tman_strerror());

    strcat(pgnexec, tmancfg->base.pgn);
    strcat(pgnexec, "/");
    strcat(pgnexec, pgn);
    strcat(pgnexec, "/");
    strcat(pgnexec, pgn);

    strcat(pgnexec, " -T ");
    strcat(pgnexec, tmancfg->base.task);
    strcat(pgnexec, " -P ");
    strcat(pgnexec, tmancfg->base.pgn);

    strcat(pgnexec, " ");
    strcat(pgnexec, pgnopts);

    if (args.prj != NULL) {
        strcat(pgnexec, " -p ");
        strcat(pgnexec, args.prj);
    }
    if (args.brd != NULL) {
        strcat(pgnexec, " -b ");
        strcat(pgnexec, args.brd);
    }
    if (args.id != NULL) {
        strcat(pgnexec, " -i ");
        strcat(pgnexec, args.id);
    }

    dlog(1, "pgnexec: %s", pgnexec);
    return system(pgnexec);
}
