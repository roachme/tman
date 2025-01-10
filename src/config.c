#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "errmod.h"
#include "common.h"
#include "osdep.h"
#include "config.h"

// TODO: gotta define default columns: curr, prev, blog, done
// TODO: gotta add config checker so a program doesn't fail.

struct config config = {
    .usehooks = TRUE,
    .hooks.size = 0,
    .columns.size = 0,
};
static const char *delim = " =\n";

static int parsepath(char *path)
{
    strcpy(path, strtok(NULL, delim));
    return 0;
}

static int parse_usehooks(const char *confkey, int *usehooks)
{
    char *confval = strtok(NULL, delim);

    if (strncmp(confval, "true", 4) == 0) {
        *usehooks = TRUE;
        return 0;
    }
    else if (strncmp(confval, "false", 5) == 0) {
        *usehooks = FALSE;
        return 0;
    }
    return elog(1, "'%s': invalid usehooks value", confval);
}

static int parse_hook(const char *hookname, struct pgns *hooks)
{
    int i = hooks->size;
    strcpy(hooks->pgn[i].pgntag, hookname);
    strcpy(hooks->pgn[i].cmd, strtok(NULL, delim));
    strcpy(hooks->pgn[i].pgname, strtok(NULL, delim));
    strcpy(hooks->pgn[i].pgncmd, strtok(NULL, delim));
    ++hooks->size;
    return 0;
}

static int parse_columns(struct columns *columns)
{
    char *prio;
    int i = columns->size;

    strcpy(columns->column[i].env, strtok(NULL, delim));
    strcpy(&columns->column[i].mark, strtok(NULL, delim));
    strcpy(columns->column[i].col, strtok(NULL, delim));
    prio = strtok(NULL, delim);
    if (prio != NULL)
        columns->column[i].prio = atoi(prio);
    return 0;
}

static int parseconf(const char *fname)
{
    FILE *fp;
    int retcode = 0;
    char line[BUFSIZ + 1];
    char *token = NULL;

    if ((fp = fopen(fname, "r")) == NULL)
        return elog(1, "could not open config file");

    while (retcode == TMAN_OK && fgets(line, BUFSIZ, fp) != NULL) {
        token = strtok(line, delim);
        if (!token || strlen(token) == 0 || token[0] == '\n' || token[0] == '#')
            continue;
        else if (config.hooks.size >= CONF_MAXHOOK)
            return elog(1, "Too many hooks in config");
        else if (config.columns.size >= CONF_MAXCOLDEF)
            return elog(1, "Too many columns per env in config");

        if (strcmp(token, "TMANBASE") == 0)
            retcode = parsepath(config.base);
        else if (strcmp(token, "TMANPGNINS") == 0)
            retcode = parsepath(config.pgnins);
        else if (strcmp(token, "USEHOOKS") == 0)
            retcode = parse_usehooks(token, &config.usehooks);
        else if (strcmp(token, "HOOKCMD") == 0)
            retcode = parse_hook(token, &config.hooks);
        else if (strcmp(token, "HOOKCAT") == 0)
            retcode = parse_hook(token, &config.hooks);
        else if (strcmp(token, "HOOKLIST") == 0)
            retcode = parse_hook(token, &config.hooks);
        else if (strcmp(token, "COLUMN") == 0)
            retcode = parse_columns(&config.columns);
        else {
            retcode = TMAN_ECONF;
            break;
        }
    }
    fclose(fp);
    return retcode;
}

int config_init(void)
{
    char *homedir = getenv("HOME");
    char configs[NUMCONFIG][CONFIGSIZ + 1] = { 0 };
    const char cfgfmts[NUMCONFIG][CONFIGSIZ + 1] = {
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
