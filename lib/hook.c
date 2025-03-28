#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tman.h"               // for tmanfs. make it not global
#include "hook.h"
#include "unit.h"
#include "osdep.h"
#include "config.h"             // for usehooks. make it not global

int ispgn(const char *pgn)
{
    char path[PATHSIZ + 1];
    sprintf(path, "%s/%s/%s", tmanfs.pgnins, pgn, pgn);
    return ISFILE(path);
}

int hookact(char *cmd, char *prj, char *id)
{
    int i;

    if (config.usehooks == FALSE)
        return 0;

    for (i = 0; i < config.hooks.size; ++i) {
        struct hook *hook = &config.hooks.hook[i];
        if (strcmp(cmd, hook->cmd) == 0)
            system(genpath_pgn(prj, id, hook->pgname, hook->pgncmd));
    }
    return 0;
}

struct unit *hookshow(char *prj, char *id)
{
    int i;
    FILE *pipe;
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    char line[BUFSIZ + 1] = { 0 };
    struct unit *unitpgn = NULL;

    if (config.usehooks == FALSE)
        return NULL;

    for (i = 0; i < config.hooks.size; ++i) {
        struct hook *hook = &config.hooks.hook[i];
        if (strcmp(hook->cmd, "show") != 0)
            continue;

        if ((pipe =
             popen(genpath_pgn(prj, id, hook->pgname, hook->pgncmd),
                   "r")) == NULL) {
            continue;
        }
        while (fgets(line, BUFSIZ, pipe)) {
            sscanf(line, "%s : %[^\n]s", key, val);
            unitpgn = unit_addpgn(unitpgn, key, val);
        }
        pclose(pipe);
    }
    return unitpgn;
}

char *hookls(char *pgnout, char *prj, char *id)
{
    int i;
    FILE *pipe;
    char *prefix = "  ";
    char line[BUFSIZ + 1] = { 0 };

    if (config.usehooks == FALSE)
        return pgnout;

    for (i = 0; i < config.hooks.size; ++i) {
        struct hook *hook = &config.hooks.hook[i];
        if (strcmp(hook->cmd, "list") != 0)
            continue;

        if ((pipe =
             popen(genpath_pgn(prj, id, hook->pgname, hook->pgncmd),
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
