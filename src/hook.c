#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include "env.h"
#include "errmod.h"
#include "task.h"
#include "tman.h"
#include "hook.h"
#include "unit.h"
#include "osdep.h"
#include "common.h"
#include "config.h"

#define FMTPGN "%s/%s/%s -e %s -i %s -b %s %s"

static char fullcmd[HOOKSIZ + 1];

static char *gen_pgncmd(char *env, char *id, char *name, char *cmd)
{
    sprintf(fullcmd, FMTPGN, tmanfs.pgnins, name, name, env, id, tmanfs.base, cmd);
    return fullcmd;
}

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
//int plugin(int argc, char **argv)
int plugin(char *env, char *id, char *pgname, char *pgncmd)
{
    char *taskenv, *taskid;

    taskenv = env;
    taskid = id;

    if (taskenv == NULL && (taskenv = env_getcurr()) == NULL)
        return emod_set(TMAN_ENV_NOCURR);
    else if (_chkenv(taskenv) == FALSE)
        return emod_set(TMAN_ENV_ILLEG);
    else if (env_exists(taskenv) == FALSE)
        return emod_set(TMAN_ENV_NOSUCH);

    else if (taskid == NULL && (taskid = tman_id_getcurr(NULL, taskenv)) == NULL)
        return emod_set(TMAN_ID_NOCURR);
    else if (_chkid(taskid) == FALSE)
        return emod_set(TMAN_ID_ILLEG);
    else if (task_ext(taskenv, taskid) == FALSE)
        return emod_set(TMAN_ID_NOSUCH);

    return system(gen_pgncmd(taskenv, taskid, pgname, pgncmd));
}

int hookact(char *cmd, char *env, char *id)
{
    int i;

    if (config.usehooks == FALSE)
        return 0;

    for (i = 0; i < config.hooks.size; ++i) {
        struct hook *hook = &config.hooks.hook[i];
        if (strcmp(cmd, hook->cmd) == 0)
            system(gen_pgncmd(env, id, hook->pgname, hook->pgncmd));
    }
    return 0;
}

struct unitpgn *hookcat(struct unitpgn *unitpgn, char *env, char *id)
{
    int i;
    FILE *pipe;
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    char line[BUFSIZ + 1] = {0};

    if (config.usehooks == FALSE)
        return unitpgn;

    for (i = 0; i < config.hooks.size; ++i) {
        struct hook *hook = &config.hooks.hook[i];
        if (strcmp(hook->cmd, "cat") != 0)
            continue;

        if ((pipe = popen(gen_pgncmd(env, id, hook->pgname, hook->pgncmd), "r")) == NULL) {
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
    int i;
    FILE *pipe;
    char *prefix = "";
    char line[BUFSIZ + 1] = {0};

    if (config.usehooks == FALSE)
        return pgnout;

    for (i = 0; i < config.hooks.size; ++i) {
        struct hook *hook = &config.hooks.hook[i];
        if (strcmp(hook->cmd, "list") != 0)
            continue;

        if ((pipe = popen(gen_pgncmd(env, id, hook->pgname, hook->pgncmd), "r")) == NULL) {
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
