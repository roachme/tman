/*
 * Module for 'special' environment names - current and previous.
 * Save these environment names to be able to switch between them.
 * They also can be used as defaults.
*/

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "env.h"
#include "common.h"
#include "osdep.h"

#define CENV        0  /* index of current env */
#define PENV        1  /* index of previous env */
#define NENV        2  /* number of 'special' environments */
#define ENVFMT      "%s\n" /* environment file format */
#define NENVITEM    1  /* number of environment items per line */

static char *envfile;
static char envs[NENV][ENVSIZ + 1];

/* Holders for caller functions. This way caller can't mess with module.  */
static char curr[ENVSIZ + 1], prev[ENVSIZ + 1];

static int load(void)
{
    int i;
    FILE *fp;

    if ((fp  = fopen(envfile, "r")) == NULL)
        return elog(1, "could not load env state");

    for (i = 0; i < NENV && fscanf(fp, ENVFMT, envs[i]) == NENVITEM; ++i)
        ;
    return fclose(fp);
}

static int save(void)
{
    int i;
    FILE *fp;

    if ((fp  = fopen(envfile, "w")) == NULL)
        return elog(1, "could not save env state");

    for (i = 0; i < NENV && envs[i][0] != '\0'; ++i)
        fprintf(fp, ENVFMT, envs[i]);
    return fclose(fp);
}

int env_init(char *fstate)
{
    envfile = fstate;

    assert(fstate != NULL && "env state file not passed or NULL");
    return load();
}

int env_isvalid(char *env)
{
    if (!isalnum(*env++))
        return 0;
    for (; *env; ++env)
        if (!(isalnum(*env) || *env == '_' || *env == '-'))
            return 0;
    return isalnum(*--env);
}

int env_exists(char *env)
{
    return ISDIR(genpath_env(env));
}

int env_reset(void)
{
    int i;

    for (i = 0; i < NENV; ++i)
        memset(envs[i], 0, ENVSIZ);
    return 0;
}

char *env_getcurr()
{
    if (envs[CENV][0] == '\0')
        return NULL;
    return strncpy(curr, envs[CENV], ENVSIZ);
}

char *env_getprev()
{
    if (envs[PENV][0] == '\0')
        return NULL;
    return strncpy(prev, envs[PENV], ENVSIZ);
}

int env_addcurr(char *env)
{
    if (strcmp(envs[CENV], env) == 0)
        return 1;
    strncpy(envs[PENV], envs[CENV], ENVSIZ);
    strncpy(envs[CENV], env, ENVSIZ);
    return save();
}

int env_delcurr(void)
{
    strncpy(envs[CENV], envs[PENV], ENVSIZ);
    memset(envs[PENV], 0, ENVSIZ);
    return save();
}

int env_delprev(void)
{
    memset(envs[PENV], 0, ENVSIZ);
    return save();
}

int env_swapenvs(void)
{
    char tmp[ENVSIZ + 1];

    if (envs[CENV][0] == '\0' || envs[PENV][0] == '\0')
        return 1;
    strncpy(tmp, envs[CENV], ENVSIZ);
    strncpy(envs[CENV], envs[PENV], ENVSIZ);
    strncpy(envs[PENV], tmp, ENVSIZ);
    return save();
}

int env_iscurr(char *env)
{
    if (env == NULL)
        return FALSE;
    if (strncmp(env, env_getcurr(), ENVSIZ) == 0)
        return TRUE;
    return FALSE;
}

int env_isprev(char *env)
{
    if (env == NULL)
        return FALSE;
    if (strncmp(env, env_getprev(), ENVSIZ) == 0)
        return TRUE;
    return FALSE;
}
