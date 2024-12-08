#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "env.h"
#include "common.h"

static char *envfile;
static char envs[NENV][ENVSIZ + 1];

static int envload(void)
{
    FILE *fp;

    if ((fp  = fopen(envfile, "r")) == NULL)
        return elog(1, "could not load env state\n");

    for (int i = 0; i < NENV && fscanf(fp, "%s", envs[i]) == NENVITEM; ++i)
        ;
    return fclose(fp);
}

static int envsave(void)
{
    FILE *fp;

    if ((fp  = fopen(envfile, "w")) == NULL)
        return elog(1, "could not save env state\n");

    for (int i = 0; i < NENV && envs[i][0] != '\0'; ++i)
        fprintf(fp, "%s\n", envs[i]);
    return fclose(fp);
}

int env_init(char *fstate)
{
    envfile = fstate;

    assert(fstate != NULL && "env state file not passed or NULL");
    return envload();
}

int env_reset(void)
{
    for (int i = 0; i < NENV; ++i)
        memset(envs[i], 0, ENVSIZ);
    return 0;
}

char *env_getcurr()
{
    if (envs[CENV][0] == '\0')
        return NULL;
    return envs[CENV];
}

char *env_getprev()
{
    if (envs[PENV][0] == '\0')
        return NULL;
    return envs[PENV];
}

int env_addcenv(char *env)
{
    if (strcmp(envs[CENV], env) == 0)
        return 1;
    strncpy(envs[PENV], envs[CENV], ENVSIZ);
    strncpy(envs[CENV], env, ENVSIZ);
    return envsave();
}

int env_delcenv(void)
{
    strncpy(envs[CENV], envs[PENV], ENVSIZ);
    memset(envs[PENV], 0, ENVSIZ);
    return envsave();
}

int env_delpenv(void)
{
    memset(envs[PENV], 0, ENVSIZ);
    return envsave();
}

int env_swapenvs(void)
{
    char tmp[ENVSIZ + 1];

    if (envs[CENV][0] == '\0' || envs[PENV][0] == '\0')
        return 1;
    strncpy(tmp, envs[CENV], ENVSIZ);
    strncpy(envs[CENV], envs[PENV], ENVSIZ);
    strncpy(envs[PENV], tmp, ENVSIZ);
    return envsave();
}
