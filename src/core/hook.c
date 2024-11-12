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

static char *hooks[] = {
    "HOOKCMD = add struct create",
    "HOOKCMD = add gun sync",
    "HOOKCMD = prev gun sync",
    "HOOKCMD = sync gun sync",
    "HOOKCAT = cat time cat",
    "HOOKCAT = cat gun cat",
    "HOOKLIST = list tag list",
};

int hooknum = sizeof(hooks) / sizeof(hooks[0]);

int isplugin(char *pgn)
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

int pgngen(char *command, char *env, char *id, char *pgname, char *pgncmd)
{
    char pathname[BUFSIZ];
    sprintf(pathname, "%s/%s/%s", tmanfs.pgnins, pgname, pgname);
    sprintf(command, "%s -e %s -i %s -b %s %s", pathname, env, id, tmanfs.base, pgncmd);
    return 0;
}

int hookact(char *command, char *env, char *id)
{
    char cmd[20];
    char pgn[20];
    char pgncmd[20];

    for (int i = 0; i < hooknum; ++i) {
        sscanf(hooks[i], "HOOKCMD = %s %s %s", cmd, pgn, pgncmd);
        if (strcmp(command, cmd) == 0) {
            char fullcmd[BUFSIZ];
            pgngen(fullcmd, env, id, pgn, pgncmd);
            system(fullcmd);
        }
    }
    return 0;
}

struct punit *hookcat(struct punit *unit, char *env, char *id)
{
    char cmd[20] = {0};
    char pgn[20] = {0};
    char pgncmd[20] = {0};
    char line[BUFSIZ] = {0};
    char fullcmd[BUFSIZ] = {0};

    unit->size = 0;
    memset(unit, 0, sizeof(struct unit));
    int pidx = 0; // cuz a plugin can output more than one lines

    for (int i = 0; i < hooknum; ++i) {
        sscanf(hooks[i], "HOOKCAT = %s %s %s", cmd, pgn, pgncmd);
        if (strcmp(cmd, "cat") != 0)
            continue;

        if (pgngen(fullcmd, env, id, pgn, pgncmd)) {
            continue;
        }
        FILE *pipe = popen(fullcmd, "r");
        if (!pipe) {
            elog(1, "hookcat: failed: '%s'", fullcmd);
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
    char cmd[BUFSIZ] = {0};
    char pgncmd[BUFSIZ] = {0};
    char line[BUFSIZ] = {0};
    char fullcmd[BUFSIZ] = {0};
    char pgn[BUFSIZ] = {0};

    char *prefix = "";
    for (int i = 0; i < hooknum; ++i) {
        sscanf(hooks[i], "HOOKLIST = %s %s %s", cmd, pgn, pgncmd);

        if (strcmp(cmd, "list") != 0) {
            continue;
        }

        if (pgngen(fullcmd, env, id, pgn, pgncmd)) {
            continue;
        }
        FILE *pipe = popen(fullcmd, "r");
        if (!pipe) {
            elog(1, "hookls: failed: '%s'", fullcmd);
            return NULL;
        }

        if (fgets(line, BUFSIZ, pipe)) {
            line[strcspn(line, "\n")] = 0;
            strcpy(pgnout + strlen(pgnout), prefix);
            strcpy(pgnout + strlen(pgnout), line);
            prefix = " ";
        }

        // roach: memory leak??? For some reasons calls other types of hooks.
        memset(cmd, 0, sizeof(cmd));

        pclose(pipe);
    }
    return pgnout;
}
