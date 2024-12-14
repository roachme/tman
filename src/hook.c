#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "hook.h"
#include "tman.h"
#include "unit.h"
#include "osdep.h"
#include "common.h"
#include "config.h"


static char fullcmd[HOOKSIZ + 1];

int isplugin(const char *pgn)
{
    char path[PATHSIZ + 1];
    sprintf(path, "%s/%s/%s", tmanfs.pgnins, pgn, pgn);
    return ISFILE(path);
}

/*
    pgn cmd [OPTION].. ID ENV
*/
// TODO: Let tman core pass plugin options to plugins
int plugin(int argc, char **argv)
{
    char c;
    char pgn[BUFSIZ + 1];
    char path[PATHSIZ + 1];
    char *name = argc > 1 ? argv[1] : "";
    char *subcmd = argc > 2 ? argv[2] : "";
    char *id = NULL, *env = NULL;
    sprintf(path, "%s/%s/%s", tmanfs.pgnins, name, name);

    while ((c = getopt(argc, argv, ":e:")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break ;
            case 'i':
                id = optarg; break ;
        }
    }

    id = argc > 4 ? argv[4] : column_getcid();
    env = argc > 5 ? argv[5] : column_getcenv();

    // TODO: check that ID and env exist.
    if (env == NULL)
        return elog(1, "no current env set");
    else if (id == NULL)
        return elog(1, "no current id set");

    // pgname ENV ID base subcmd [OPTION]..
    sprintf(pgn, "%s -e %s -i %s -b %s %s", path, env, id, tmanfs.base, subcmd);
    system(pgn);
    return 1;
}

static char *cmdgen(struct hook *hook, char *env, char *id)
{
    sprintf(fullcmd, "%s/%s/%s -e %s -i %s -b %s %s",
            config.pgnins, hook->pgname, hook->pgname,
            env, id, config.base, hook->pgncmd);
    return fullcmd;
}

int hookact(char *cmd, char *env, char *id)
{
    if (config.nohooks == TRUE)
        return 0;
    for (int i = 0; i < config.hooks.size; ++i) {
        struct hook *hook = &config.hooks.hook[i];
        if (strcmp(cmd, hook->cmd) == 0)
            system(cmdgen(hook, env, id));
    }
    return 0;
}

struct unitpgn *hookcat(struct unitpgn *unitpgn, char *env, char *id)
{
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    char line[BUFSIZ + 1] = {0};
    int pidx = 0; // cuz a plugin can output more than one lines

    if (config.nohooks == TRUE)
        return unitpgn;
    for (int i = 0; i < config.hooks.size; ++i) {
        struct hook *hook = &config.hooks.hook[i];
        if (strcmp(hook->cmd, "cat") != 0)
            continue;

        FILE *pipe = popen(cmdgen(hook, env, id), "r");
        if (!pipe) {
            elog(1, "hookcat: failed: '%s'", fullcmd);
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

char *hookls(char *pgnout, char *env, char *id)
{
    char *prefix = "";
    char line[BUFSIZ + 1] = {0};

    if (config.nohooks == TRUE)
        return pgnout;
    for (int i = 0; i < config.hooks.size; ++i) {
        struct hook *hook = &config.hooks.hook[i];
        if (strcmp(hook->cmd, "list") != 0)
            continue;

        FILE *pipe = popen(cmdgen(hook, env, id), "r");
        if (!pipe) {
            elog(1, "hookls: failed: '%s'", fullcmd);
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
