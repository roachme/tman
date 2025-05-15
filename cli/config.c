#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../lib/osdep.h"
#include "config.h"
#include "cli.h"

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
    return emod_set(LIBTMAN_NODEF_ERR);
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
        return hooks;
    }

    strcpy(hook->pgntag, hookname);
    strcpy(hook->cmd, strtok(NULL, delim));
    strcpy(hook->pgname, strtok(NULL, delim));
    strcpy(hook->pgncmd, strtok(NULL, delim));
    hook->next = hooks;
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

static int parseconf(struct config *myconfig, const char *fname)
{
    FILE *fp;
    int retcode = 0;
    char line[BUFSIZ + 1];
    char *token = NULL;

    if ((fp = fopen(fname, "r")) == NULL) {
        fprintf(stderr, "could not open config file");
        return 1;
    }

    while (retcode == LIBTMAN_OK && fgets(line, BUFSIZ, fp) != NULL) {
        token = strtok(line, delim);
        if (!token || strlen(token) == 0 || token[0] == '\n' || token[0] == '#')
            continue;

        if (strcmp(token, "TMANBASE") == 0)
            retcode = parsepath(myconfig->base);
        else if (strcmp(token, "TMANPGNINS") == 0)
            retcode = parsepath(myconfig->pgnins);
        else if (strcmp(token, "USEHOOKS") == 0)
            retcode = parse_usehooks(token, &myconfig->usehooks);
        else if (strcmp(token, "HOOKCMD") == 0)
            myconfig->hooks = parse_hook(token, myconfig->hooks);
        else if (strcmp(token, "HOOKCAT") == 0)
            myconfig->hooks = parse_hook(token, myconfig->hooks);
        else if (strcmp(token, "HOOKLIST") == 0)
            myconfig->hooks = parse_hook(token, myconfig->hooks);
        else if (strcmp(token, "COLUMN") == 0) {
            // TODO: add a parser
        } else {
            // TODO: caller can't know that something went wrong
            // cuz callee doesn't return an error or NULL for now.
            elog(1, "parse err: unrecognized token '%s'", token);
            break;
        }
    }
    fclose(fp);
    return 0;
}

struct config *config_init(void)
{
    struct config *config;

    if ((config = malloc(sizeof(struct config))) == NULL)
        return 0;
    memset(config, 0, sizeof(struct config));
    return config;
}

int config_parse(struct config *config)
{
    int i;
    const char *progname = "tman";
    char *homedir = getenv("HOME");
    char cfgfile[CONFIGSIZ + 1];
    const char cfgfmts[NUMCONFIG][CONFIGSIZ + 1] = {
        "%s/.%s/%s.cfg",
        "%s/.config/%s/%s.cfg",
    };

    for (i = 0; i < NUMCONFIG; ++i) {
        sprintf(cfgfile, cfgfmts[i], homedir, progname, progname);
        if (ISFILE(cfgfile)) {
            return parseconf(config, cfgfile);
        }
    }
    return 1;
}

void *config_deinit(struct config *config)
{
    struct tman_hook *head;

    for (head = config->hooks; head != NULL;) {
        struct tman_hook *tmp = head;
        head = head->next;
        free(tmp);
    }
    free(config);
    return NULL;
}
