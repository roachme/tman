#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

#include "hook.h"
#include "config.h"
#include "../tec.h"
#include "../../lib/libtec.h"

static char pathname[PATH_MAX + 1];

static char *path_pgn(tec_arg_t * args, char *name, char *cmd)
{
    const char *fmt = "%s/%s/%s -T %s -P %s %s -p %s -b %s -i %s";
    sprintf(pathname, fmt, teccfg.base.pgn, name, name, teccfg.base.task,
            teccfg.base.pgn, cmd, args->project, args->board, args->taskid);
    return pathname;
}

int hook_action(tec_arg_t * args, char *cmd)
{
    struct tec_hook *hooks = teccfg.hooks;

    /* Execute hooks only if they are enabled.  */
    if (teccfg.opts.hook == false)
        return 0;

    for (; hooks; hooks = hooks->next) {
        if (strcmp(cmd, hooks->cmd) == 0) {
            char *cmd = path_pgn(args, hooks->pgname, hooks->pgncmd);
            dlog(1, cmd);
            system(cmd);
        }
    }
    return 0;
}

int hook_show(tec_unit_t ** units, tec_arg_t * args, char *cmd)
{
    FILE *pipe;
    char line[BUFSIZ + 1] = { 0 };
    struct tec_hook *hooks = teccfg.hooks;

    /* Execute hooks only if they are enabled.  */
    if (teccfg.opts.hook == false)
        return 0;

    for (; hooks; hooks = hooks->next) {
        if (strcmp(hooks->cmd, cmd) != 0)
            continue;

        if (!(pipe = popen(path_pgn(args, hooks->pgname, hooks->pgncmd), "r"))) {
            // TODO: add quiet option and show error message
            continue;
        }
        while (fgets(line, BUFSIZ, pipe))
            *units = tec_unit_parse(*units, line);
        pclose(pipe);
    }
    return 0;
}

/*
char *hook_list(struct tec_hook *hooks, char *pgnout, char *project, char *task)
{
    FILE *pipe;
    char *prefix = "  ";
    char line[BUFSIZ + 1] = { 0 };

    // Execute hooks only if they are enabled.
    if (teccfg.opts.hook == true)
        return 0;

    for (; hooks; hooks = hooks->next) {
        if (strcmp(hooks->cmd, "list") != 0)
            continue;

        if ((pipe =
             popen(genpath_pgn(project, task, hooks->pgname, hooks->pgncmd),
                   "r")) == NULL) {
            return NULL;
        }
        // NOTE: gotta get a single word
        if (fgets(line, BUFSIZ, pipe)) {
            line[strcspn(line, "\n")] = 0;
            strcat(pgnout, prefix);
            strcat(pgnout, line);
        }

        pclose(pipe);
    }

    if (pgnout[1] == ' ') {
        pgnout[1] = '[';
        strcat(pgnout, "]");
    }
    //return LIBTEC_OK;
    return pgnout;
}
*/

/* TODO:
    Structure: tec PGN -i -b -p COMMAND [OPTION]... [APRS]
    1. Can't use getopt cuz there might be different options for plugins and
       their commads.
    2. Use for/while loop to circle through options and their arguments.
    3. Separate plugin options from plugin command options.
    4. Or maybe it's better to let the plugin to handle plugin options and the rest.
    5. Add a better parser to include all the other tec options like -C, -H, -F, etc.
*/
int tec_cli_plugin(int argc, char **argv, tec_ctx_t * ctx)
{
    int i;
    char *pgn;
    char pgnexec[BUFSIZ + 1] = { 0 };

    i = 0;
    pgn = argv[i++];

    sprintf(pgnexec, "%s/%s/%s -T %s -P %s ", teccfg.base.pgn, pgn, pgn,
            teccfg.base.task, teccfg.base.pgn);

    for (; i < argc; ++i) {
        strcat(pgnexec, argv[i]);
        strcat(pgnexec, " ");
    }

    dlog(1, "pgnexec: %s", pgnexec);
    return system(pgnexec);
}
