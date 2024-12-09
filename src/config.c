#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "common.h"
#include "osdep.h"
#include "config.h"

// TODO: gotta define default columns: curr, prev, blog, done
// TODO: gotta add config checker so a program doesn't fail.

struct config config = {
    //.coldefs.coldef[0] = { .prio = 0, },
};

static int parsepath(char *path)
{
    const char *delim = " =\n";
    strcpy(path, strtok(NULL, delim));
    return 0;
}

static int parse_hook(const char *hookname, struct hooks *hooks)
{
    int i = hooks->size;
    const char *delim = " =\n";
    strcpy(hooks->hook[i].hook, hookname);
    strcpy(hooks->hook[i].cmd, strtok(NULL, delim));
    strcpy(hooks->hook[i].pgname, strtok(NULL, delim));
    strcpy(hooks->hook[i].pgncmd, strtok(NULL, delim));
    ++hooks->size;
    return 0;
}

static int parse_columns(struct columns *columns)
{
    char *prio;
    int i = columns->size;
    const char *delim = " =\n";

    strcpy(columns->column[i].env, strtok(NULL, delim));
    strcpy(&columns->column[i].mark, strtok(NULL, delim));
    strcpy(columns->column[i].tag, strtok(NULL, delim));
    prio = strtok(NULL, delim);
    if (prio != NULL)
        columns->column[i].prio = atoi(prio);
    return 0;
}

int parseconf(const char *fname)
{
    char line[BUFSIZ + 1];
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
        else if (config.hooks.size >= CONF_MAXHOOK)
            return elog(1, "Too many hooks in config");
        else if (config.columns.size >= CONF_MAXCOLDEF)
            return elog(1, "Too many columns per env in config");

        if (strcmp(token, "TMANBASE") == 0)
            parsepath(config.base);
        else if (strcmp(token, "TMANPGNINS") == 0)
            parsepath(config.pgnins);
        else if (strcmp(token, "HOOKCMD") == 0)
            parse_hook(token, &config.hooks);
        else if (strcmp(token, "HOOKCAT") == 0)
            parse_hook(token, &config.hooks);
        else if (strcmp(token, "HOOKLIST") == 0)
            parse_hook(token, &config.hooks);
        else if (strcmp(token, "COLUMN") == 0) {
            parse_columns(&config.columns);
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
    char *homedir = getenv("HOME");
    char configs[NUMCONFIG][MAXCONFIG] = {0};
    const char cfgfmts[NUMCONFIG][MAXCONFIG] = {
        "%s/.%s/%s.cfg",
        "%s/.config/%s/%s.cfg",
    };

    for (int i = 0; i < NUMCONFIG; ++i) {
        sprintf(configs[i], cfgfmts[i], homedir, PROGRAM, PROGRAM);
        if (ISFILE(configs[i]))
            return parseconf(configs[i]);
    }
    return elog(1, "could not find system config file");
}
