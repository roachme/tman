#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

    if (!fp) {
        fprintf(stderr, "could not open config file\n");
        return 1;
    }

    while (fgets(line, BUFSIZ, fp) != NULL) {
        token = strtok(line, delim);
        if (!token || strlen(token) == 0 || token[0] == '\n' || token[0] == '#')
            continue;

        hook = &config.hooks[config.hooknum++];
        if (strcmp(token, "TMANBASE") == 0)
            parsepath(config.base);
        else if (strcmp(token, "TMANPGNINS") == 0)
            parsepath(config.pgnins);
        else if (strcmp(token, "HOOKCMD") == 0)
            parsehook(token, hook);
        else if (strcmp(token, "HOOKCAT") == 0)
            parsehook(token, hook);
        else if (strcmp(token, "HOOKLIST") == 0)
            parsehook(token, hook);
        else {
            fprintf(stderr, "not found %s: unknown variable\n", token);
            return 1;
        }
    }
    return 0;
}

int findconfig()
{
    char homedir[BUFSIZ];
    sprintf(homedir, "%s", getenv("HOME"));
    char *configs[] = {};
    return 0;
}
