#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "osdep.h"
#include "config.h"

struct config config;

static int parsepath(char *path)
{
    const char *delim = " =\n";
    strcpy(path, strtok(NULL, delim));
    return 0;
}

static int parsehook(const char *hookname, struct hook *hookcmd)
{
    const char *delim = " =\n";
    strcpy(hookcmd->hook, hookname);
    strcpy(hookcmd->cmd, strtok(NULL, delim));
    strcpy(hookcmd->pgname, strtok(NULL, delim));
    strcpy(hookcmd->pgncmd, strtok(NULL, delim));
    return 0;
}

int parseconf(const char *fname)
{
    char line[BUFSIZ];
    char *token = NULL;
    struct hook *hook;
    const char *delim = " =\n";
    FILE *fp = fopen(fname, "r");

    if (!fp)
        return elog(1, "could not open config file");

    while (fgets(line, BUFSIZ, fp) != NULL) {
        token = strtok(line, delim);
        if (!token || strlen(token) == 0 || token[0] == '\n' || token[0] == '#')
            continue;

        if (config.hooknum >= CONF_MAXHOOK)
            return elog(1, "Too many hooks in config");

        hook = &config.hooks[config.hooknum];

        if (strcmp(token, "TMANBASE") == 0)
            parsepath(config.base);
        else if (strcmp(token, "TMANPGNINS") == 0)
            parsepath(config.pgnins);
        else if (strcmp(token, "HOOKCMD") == 0) {
            parsehook(token, hook);
            ++config.hooknum;
        }
        else if (strcmp(token, "HOOKCAT") == 0) {
            parsehook(token, hook);
            ++config.hooknum;
        }
        else if (strcmp(token, "HOOKLIST") == 0) {
            parsehook(token, hook);
            ++config.hooknum;
        }
        else {
            fprintf(stderr, "not found %s: unknown variable\n", token);
            return 1;
        }
    }
    return fclose(fp);
}

int config_init(void)
{
    char *fconfig = NULL;
    char *homedir = getenv("HOME");
    char configs[NUMCONFIG][MAXCONFIG] = {0};

    sprintf(configs[0], "%s/.%s/%s.cfg", homedir, PROGRAM, PROGRAM);
    sprintf(configs[1], "%s/.config/%s/%s.cfg", homedir, PROGRAM, PROGRAM);

    for (int i = 0; i < NUMCONFIG; ++i) {
        if (FCHK(configs[i]) == 0) {
            fconfig = configs[i];
            break;
        }
    }

    if (fconfig == NULL)
        return elog(1, "could not find system config file");

    return parseconf(fconfig);
}
