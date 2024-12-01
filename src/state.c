#include <string.h>

#include "state.h"
#include "column.h"
#include "common.h"

static char *_fstate, *_db;
static struct envstate envs[NENV];

static int _reset()
{
    memset(envs, 0, sizeof(envs));
    return 0;
}

static int _load(void)
{
    FILE *fp;
    char line[BUFSIZ];
    char envpath[BUFSIZ];

    if ((fp = fopen(formpath(envpath, FMTSTATE, _db), "r")) == NULL)
        return elog(1, "could not open env state file %s\n", envpath);
    else if (_reset())
        return elog(1, "could not reset static var envs\n");

    for (int i = 0; i < NENV && fgets(line, BUFSIZ, fp) != NULL; ++i)
        sscanf(line, "%s", envs[i].name);
    return fclose(fp);
}

static int _savenv(void)
{
    FILE *fp;

    if ((fp = fopen(_fstate, "w")) == NULL)
        return elog(1, "could not save env state");

    for (int i = 0; i < NENV; ++i)
        fprintf(fp, "%s\n", envs[i].name);
    return fclose(fp);
}

int state_init(char *fstate, char *db)
{
    if ((_fstate = fstate) == NULL)
        return elog(1, "state file is NULL");
    else if ((_db = db) == NULL)
        return elog(1, "db dirpath is NULL");
    else if (_load())
        return elog(1, "could not load env");
    return 0;
}

char *state_getcid()
{
    if (envs[CENV].name[0] == '\0')
        return NULL;
    else if (envs[CENV].curr[0] == '\0')
        return NULL;
    return envs[CENV].curr;
}

char *state_getpid()
{
    if (envs[CENV].name[0] == '\0')
        return NULL;
    else if (envs[CENV].prev[0] == '\0')
        return NULL;
    return envs[CENV].prev;
}

char *state_getcenv()
{
    if (envs[CENV].name[0] == '\0')
        return NULL;
    return envs[CENV].name;
}

char *state_getpenv()
{
    if (envs[PENV].name[0] == '\0')
        return NULL;
    return envs[PENV].name;
}

/* Setters:BEGIN id */
int state_setcid(char *id)
{
    if (envs[CENV].name[0] == '\0')
        return 1;
    strcpy(envs[CENV].curr, id);
    return 0;
}

int state_setpid(char *id)
{
    if (envs[CENV].name[0] == '\0')
        return 1;
    strcpy(envs[CENV].prev, id);
    return 0;
}

int state_addcid(char *id)
{
    if (envs[CENV].name[0] == '\0' || id == NULL || id[0] == '\0')
        return 1;
    strcpy(envs[CENV].prev, envs[CENV].curr);
    strcpy(envs[CENV].curr, id);
    return 0;
}

int state_delcid()
{
    if (envs[CENV].name[0] == '\0')
        return 1;
    strcpy(envs[CENV].curr, envs[CENV].prev);
    memset(envs[CENV].prev, 0, IDSIZ);
    return 0;
}

int state_swapids()
{
    char tmp[IDSIZ + 1];

    if (envs[CENV].prev[0] == '\0')
        return 1;
    strcpy(tmp, envs[CENV].prev);
    return state_addcid(tmp);
}

int state_delpid()
{
    if (state_swapids())
        return 1;
    return state_delcid();
}

/* Setters:END id */


/* Setters:BEGIN env */

int state_addcenv(char *cenv)
{
    strcpy(envs[PENV].name, envs[CENV].name);
    strcpy(envs[CENV].name, cenv);
    return _savenv();
}

int state_delcenv(void)
{
    strcpy(envs[CENV].name, envs[PENV].name);
    memset(envs[PENV].name, 0, ENVSIZ);
    return _savenv();
}

int state_delpenv(void)
{
    memset(envs[PENV].name, 0, ENVSIZ);
    return _savenv();
}

int state_swapenvs(void)
{
    char tmp[ENVSIZ + 1];

    if (envs[CENV].name[0] == '\0' || envs[PENV].name[0] == '\0')
        return 1;

    strcpy(tmp, envs[CENV].name);
    strcpy(envs[CENV].name, envs[PENV].name);
    strcpy(envs[PENV].name, tmp);
    return _savenv();
}

/* Setters:END env */
