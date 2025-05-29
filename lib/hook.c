#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hook.h"
#include "unit.h"
#include "osdep.h"

int ispgn(char *pgndir, const char *pgname)
{
    char path[PATHSIZ + 1];
    sprintf(path, "%s/%s/%s", pgndir, pgname, pgname);
    return ISFILE(path);
}

int hookact(struct tman_hook *hooks, char *cmd, char *prj, char *id)
{
    for (; hooks; hooks = hooks->next) {
        if (strcmp(cmd, hooks->cmd) == 0) {
            char *pgncmd = genpath_pgn(prj, id, hooks->pgname, hooks->pgncmd);
            system(pgncmd);
        }
    }
    return 0;
}

struct unit *hookshow(struct tman_hook *hooks, char *prj, char *id, char *cmd)
{
    FILE *pipe;
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    char line[BUFSIZ + 1] = { 0 };
    struct unit *unitpgn = NULL;

    for (; hooks; hooks = hooks->next) {
        if (strcmp(hooks->cmd, cmd) != 0)
            continue;

        if ((pipe =
             popen(genpath_pgn(prj, id, hooks->pgname, hooks->pgncmd),
                   "r")) == NULL) {
            continue;
        }
        while (fgets(line, BUFSIZ, pipe)) {
            sscanf(line, "%s : %[^\n]s", key, val);
            unitpgn = unit_add(unitpgn, key, val);
        }
        pclose(pipe);
    }
    return unitpgn;
}

char *hookls(struct tman_hook *hooks, char *pgnout, char *prj, char *id)
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

    return pgnout;
}
