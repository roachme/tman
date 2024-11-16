#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "hook.h"
#include "tman.h"
#include "unit.h"
#include "osdep.h"
#include "state.h"
#include "common.h"
#include "config.h"


char fullcmd[HOOKSIZ];

int isplugin(const char *pgn)
{
    char path[PATHSIZ + 1];
    sprintf(path, "%s/%s/%s", tmanfs.pgnins, pgn, pgn);
    return !FCHK(path);
}

/*
    pgn cmd [OPTION].. ID ENV
*/
int plugin(int argc, char **argv)
{
    char c;
    char pgn[BUFSIZ];
    char path[PATHSIZ + 1];
    char *name = argc > 1 ? argv[1] : "";
    char *subcmd = argc > 2 ? argv[2] : "";
    char *id = NULL, *env = NULL;
    sprintf(path, "%s/%s/%s", tmanfs.pgnins, name, name);

    if (access(path, F_OK))
        return elog(1, "plugin not found -> %s", path);

    while ((c = getopt(argc, argv, ":e:")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break ;
            case 'i':
                id = optarg; break ;
        }
    }

    id = argc > 4 ? argv[4] : state_getcid();
    env = argc > 5 ? argv[5] : state_getcenv();

    // TODO: check that ID and env exist.
    if (env[0] == '\0')
        return elog(1, "no current env set");
    else if (id[0] == '\0')
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
    for (int i = 0; i < config.hooknum; ++i) {
        struct hook *hook = &config.hooks[i];
        if (strcmp(cmd, hook->cmd) == 0)
            system(cmdgen(hook, env, id));
    }
    return 0;
}

struct punit *hookcat(struct punit *unit, char *env, char *id)
{
    char *hookcmd;
    char line[BUFSIZ] = {0};
    int pidx = 0; // cuz a plugin can output more than one lines

    for (int i = 0; i < config.hooknum; ++i) {
        struct hook *hook = &config.hooks[i];
        if (strcmp(hook->cmd, "cat") != 0)
            continue;

        hookcmd = cmdgen(hook, env, id);
        FILE *pipe = popen(fullcmd, "r");
        if (!pipe) {
            elog(1, "hookcat: failed: '%s'", hookcmd);
            return NULL;
        }
        for ( ; fgets(line, BUFSIZ, pipe); ++pidx) {
            sscanf(line, "%s : %[^\n]s", unit->pair[pidx].key, unit->pair[pidx].val);
            ++unit->size;
        }
        pclose(pipe);
    }
    return unit;
}

char *hookls(char *pgnout, char *env, char *id)
{
    char *prefix = "";
    char line[BUFSIZ] = {0};
    char *hookcmd;

    for (int i = 0; i < config.hooknum; ++i) {
        struct hook *hook = &config.hooks[i];
        if (strcmp(hook->cmd, "list") != 0)
            continue;

        hookcmd = cmdgen(hook, env, id);
        FILE *pipe = popen(hookcmd, "r");
        if (!pipe) {
            elog(1, "hookls: failed: '%s'", hookcmd);
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