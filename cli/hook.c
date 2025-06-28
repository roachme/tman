#include <string.h>
#include <stdlib.h>

#include "hook.h"
#include "config.h"
#include "../lib/src/libtman.h"

int tman_hook_action(struct tman_arg *args, char *cmd)
{
    char *pgncmd;
    struct tman_hook *hooks = tmancfg->hooks;

    for (; hooks; hooks = hooks->next) {
        if (strcmp(cmd, hooks->cmd) == 0) {
            pgncmd =
                genpath_pgn(args->prj, args->id, hooks->pgname, hooks->pgncmd);
            system(pgncmd);
        }
    }
    return LIBTMAN_OK;
}

int tman_hook_show(struct tman_context *ctx, struct tman_arg *args, char *cmd)
{
    FILE *pipe;
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    char line[BUFSIZ + 1] = { 0 };
    struct tman_hook *hooks = tmancfg->hooks;

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
    return LIBTMAN_OK;
}

char *tman_hookls(struct tman_hook *hooks, char *pgnout, char *prj, char *id)
{
    FILE *pipe;
    char *prefix = "  ";
    char line[BUFSIZ + 1] = { 0 };

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
