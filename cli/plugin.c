#include <stdio.h>
#include <string.h>
#include "cli.h"

#define NPGNOPT 20

/* TODO:
    Structure: tman PGN -i -b -p COMMAND [OPTION]... [APRS]
    1. Can't use getopt cuz there might be different options for plugins and
       their commads.
    2. Use for/while loop to circle through options and their arguments.
    3. Separate plugin options from plugin command options.
    4. Or maybe it's better to let the plugin to handle plugin options and the rest.
*/
int tman_cli_plugin(char *name, int argc, char **argv, struct tman_context *ctx)
{
    int i, pgnixd, status;
    char *option, *pgncmd;
    struct tman_arg args;
    char pgnopts[BUFSIZ + 1] = { 0 };

    i = pgnixd = 0;
    args.id = args.brd = args.prj = NULL;

    /* Get plugin command.  */
    if (argv[i][0] != '-')
        pgncmd = argv[i++];
    else
        pgncmd = "";

    for (; i < argc ; ++i, ++pgnixd) {
        if ((option = argv[i]) && strcmp(option, "-b") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            args.brd = argv[++i];
            return elog(1, "option '%s' under development", option);
        } else if ((option = argv[i]) && strcmp(option, "-i") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            args.id = argv[++i];
        } else if ((option = argv[i]) && strcmp(option, "-p") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            args.prj = argv[i + 1];
        } else {
            /* preserve plugin specific options.  */
            strcat(pgnopts, argv[i]);
            strcat(pgnopts, " ");
        }
    }

    if ((status =
         tman_pgnexec(NULL, &args, name, pgncmd, NULL, pgnopts)) != LIBTMAN_OK)
        elog(status, "pgn failed: %s", tman_strerror());
    return status;
}
