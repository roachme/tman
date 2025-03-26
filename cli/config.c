#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/osdep.h"
#include "config.h"

// TODO: gotta define default columns: curr, prev, blog, done
// TODO: gotta add config checker so a program doesn't fail.

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
    } else if (strncmp(confval, "false", 5) == 0) {
        *usehooks = FALSE;
        return 0;
    }
    fprintf(stderr, "'%s': invalid usehooks value", confval);
    return emod_set(TMAN_NODEF_ERR);
}

struct tman_hook *make_hook()
{
    struct tman_hook *hook;

    if ((hook = malloc(sizeof(struct tman_hook))) == NULL)
        return NULL;
    memset(hook, 0, sizeof(struct tman_hook));
    return hook;
}

struct tman_hook *parse_hook(const char *hookname, struct tman_hook *hooks)
{
    struct tman_hook *hook;

    if ((hook = make_hook()) == NULL) {
        elog(1, "make_hook: ERR");
        return NULL;
    }

    strcpy(hook->pgntag, hookname);
    strcpy(hook->cmd, strtok(NULL, delim));
    strcpy(hook->pgname, strtok(NULL, delim));
    strcpy(hook->pgncmd, strtok(NULL, delim));
    hook->next = hooks;
    //elog(1, "->>hookcmd: %s", hook->cmd);
    return hook;
}

/*
static int parse_columns(struct columns *columns)
{
    char *prio;
    int i = columns->size;

    strcpy(columns->column[i].prj, strtok(NULL, delim));
    strcpy(&columns->column[i].mark, strtok(NULL, delim));
    strcpy(columns->column[i].col, strtok(NULL, delim));
    prio = strtok(NULL, delim);
    if (prio != NULL)
        columns->column[i].prio = atoi(prio);
    return 0;
}
*/

static struct config *parseconf(const char *fname)
{
    FILE *fp;
    int retcode = 0;
    char line[BUFSIZ + 1];
    char *token = NULL;

    if ((tman_config = malloc(sizeof(struct config))) == NULL)
        return NULL;
    memset(tman_config, 0, sizeof(struct config));

    if ((fp = fopen(fname, "r")) == NULL) {
        fprintf(stderr, "could not open config file");
        emod_set(TMAN_NODEF_ERR);
        return NULL;
    }

    while (retcode == TMAN_OK && fgets(line, BUFSIZ, fp) != NULL) {
        token = strtok(line, delim);
        if (!token || strlen(token) == 0 || token[0] == '\n' || token[0] == '#')
            continue;

        if (strcmp(token, "TMANBASE") == 0)
            retcode = parsepath(tman_config->base);
        else if (strcmp(token, "TMANPGNINS") == 0)
            retcode = parsepath(tman_config->pgnins);
        else if (strcmp(token, "USEHOOKS") == 0)
            retcode = parse_usehooks(token, &tman_config->usehooks);
        else if (strcmp(token, "HOOKCMD") == 0)
            tman_config->hooks = parse_hook(token, tman_config->hooks);
        else if (strcmp(token, "HOOKCAT") == 0)
            tman_config->hooks = parse_hook(token, tman_config->hooks);
        else if (strcmp(token, "HOOKLIST") == 0)
            tman_config->hooks = parse_hook(token, tman_config->hooks);
        else if (strcmp(token, "COLUMN") == 0) {
            // TODO: add a parser
        } else {
            // TODO: caller can't know that something went wrong
            // cuz callee doesn't return an error or NULL for now.
            retcode = TMAN_ECONF;
            elog(1, "parse err: unrecognized token '%s'", token);
            break;
        }
    }
    fclose(fp);
    return tman_config;
}

struct config *config_init(void)
{
    const char *progname = "tman";
    char *homedir = getenv("HOME");
    char cfg[CONFIGSIZ + 1];
    const char cfgfmts[NUMCONFIG][CONFIGSIZ + 1] = {
        "%s/.%s/%s.cfg",
        "%s/.config/%s/%s.cfg",
    };

    for (int i = 0; i < NUMCONFIG; ++i) {
        sprintf(cfg, cfgfmts[i], homedir, progname, progname);
        if (ISFILE(cfg)) {
            return parseconf(cfg);
        }
    }
    fprintf(stderr, "could not find system config file");
    emod_set(TMAN_NODEF_ERR);
    return NULL;
}

void *config_deinit(struct config *config)
{
    for (; config->hooks; config->hooks = config->hooks->next)
        free(config->hooks);
    free(config);
    return NULL;
}
