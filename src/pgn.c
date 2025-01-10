#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "tman.h" // for tmanfs. make it not global
#include "pgn.h"
#include "unit.h"
#include "osdep.h"
#include "config.h" // for usehooks. make it not global

int ispgn(const char *pgn)
{
    char path[PATHSIZ + 1];
    sprintf(path, "%s/%s/%s", tmanfs.pgnins, pgn, pgn);
    return ISFILE(path);
}

int pgnact(char *cmd, char *env, char *id)
{
    int i;

    if (config.usehooks == FALSE)
        return 0;

    for (i = 0; i < config.hooks.size; ++i) {
        struct pgn *hook = &config.hooks.pgn[i];
        if (strcmp(cmd, hook->cmd) == 0)
            system(genpath_pgn(env, id, hook->pgname, hook->pgncmd));
    }
    return 0;
}

struct unit *pgncat(struct unit *unitpgn, char *env, char *id)
{
    int i;
    FILE *pipe;
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    char line[BUFSIZ + 1] = {0};

    if (config.usehooks == FALSE)
        return unitpgn;

    for (i = 0; i < config.hooks.size; ++i) {
        struct pgn *hook = &config.hooks.pgn[i];
        if (strcmp(hook->cmd, "cat") != 0)
            continue;

        if ((pipe = popen(genpath_pgn(env, id, hook->pgname, hook->pgncmd), "r")) == NULL) {
            elog(1, "hookcat: failed to execute hookcat");
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

char *pgnls(char *pgnout, char *env, char *id)
{
    int i;
    FILE *pipe;
    char *prefix = "";
    char line[BUFSIZ + 1] = {0};

    if (config.usehooks == FALSE)
        return pgnout;

    for (i = 0; i < config.hooks.size; ++i) {
        struct pgn *hook = &config.hooks.pgn[i];
        if (strcmp(hook->cmd, "list") != 0)
            continue;

        if ((pipe = popen(genpath_pgn(env, id, hook->pgname, hook->pgncmd), "r")) == NULL) {
            elog(1, "hookls: failed to execute hookls");
            return NULL;
        }

        // TODO: simplify this shit
        if (fgets(line, BUFSIZ, pipe)) {
            line[strcspn(line, "\n")] = 0;
            strcpy(pgnout + strlen(pgnout), prefix);
            strcpy(pgnout + strlen(pgnout), line);
            prefix = " ";
        }

        pclose(pipe);
    }
    return pgnout;
}
