#include <string.h>
#include <stdlib.h>

#include "hook.h"
#include "config.h"
#include "../lib/src/libtman.h"

static char pathname[PATHSIZ + 1];

static char *genpath_pgn(char *prj, char *id, char *name, char *cmd)
{
    const char *fmt = "%s/%s/%s -T %s -P %s %s -i %s -p %s";
    sprintf(pathname, fmt, tmancfg->base.pgn, name, name, tmancfg->base.task,
            tmancfg->base.pgn, cmd, id, prj);
    return pathname;
}

int hook_action(tman_arg_t * args, char *cmd)
{
    char *pgncmd;
    struct tman_hook *hooks = tmancfg->hooks;

    /* Execute hooks only if they are enabled.  */
    if (tmancfg->usehooks == FALSE)
        return 0;

    for (; hooks; hooks = hooks->next) {
        if (strcmp(cmd, hooks->cmd) == 0) {
            pgncmd =
                genpath_pgn(args->prj, args->id, hooks->pgname, hooks->pgncmd);
            system(pgncmd);
        }
    }
    return 0;
}

int hook_show(tman_ctx_t * ctx, tman_arg_t * args, char *cmd)
{
    FILE *pipe;
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    char line[BUFSIZ + 1] = { 0 };
    struct tman_hook *hooks = tmancfg->hooks;

    /* Execute hooks only if they are enabled.  */
    if (tmancfg->usehooks == FALSE)
        return 0;

    for (; hooks; hooks = hooks->next) {
        if (strcmp(hooks->cmd, cmd) != 0)
            continue;

        if ((pipe =
             popen(genpath_pgn
                   (args->prj, args->id, hooks->pgname, hooks->pgncmd),
                   "r")) == NULL) {
            continue;
        }
        while (fgets(line, BUFSIZ, pipe)) {
            sscanf(line, "%s : %[^\n]s", key, val);
            ctx->unitpgn = tman_unit_add(ctx->unitpgn, key, val);
        }
        pclose(pipe);
    }
    return 0;
}

char *hook_list(struct tman_hook *hooks, char *pgnout, char *prj, char *id)
{
    FILE *pipe;
    char *prefix = "  ";
    char line[BUFSIZ + 1] = { 0 };

    /* Execute hooks only if they are enabled.  */
    if (tmancfg->usehooks == TRUE)
        return 0;

    for (; hooks; hooks = hooks->next) {
        if (strcmp(hooks->cmd, "list") != 0)
            continue;

        if ((pipe =
             popen(genpath_pgn(prj, id, hooks->pgname, hooks->pgncmd),
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
    //return LIBTMAN_OK;
    return pgnout;
}
