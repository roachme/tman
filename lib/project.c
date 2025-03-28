/*
 * Module for 'special' project names - current and previous.
 * Save these project names to be able to switch between them.
 * They also can be used as defaults.
*/

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "project.h"
#include "common.h"
#include "errmod.h"
#include "osdep.h"

#define CPRJ        0           /* index of current prj */
#define PPRJ        1           /* index of previous prj */
#define NPRJ        2           /* number of 'special' project */
#define PRJFMT      "%s\n"      /* project file format */
#define NPRJITEM    1           /* number of project items per line */

static char *prjfile;
static char prjs[NPRJ][PRJSIZ + 1];

/* Holders for caller functions. This way caller can't mess with module.  */
static char curr[PRJSIZ + 1], prev[PRJSIZ + 1];

static int is_project_set(char *prj)
{
    if (prj == NULL || prj[0] == '\0')
        return FALSE;
    return TRUE;
}

static int load(void)
{
    int i;
    FILE *fp;

    if ((fp = fopen(prjfile, "r")) == NULL)
        return emod_set(TMAN_PRJ_LOAD);

    for (i = 0; i < NPRJ && fscanf(fp, PRJFMT, prjs[i]) == NPRJITEM; ++i) ;
    return fclose(fp);
}

static int save(void)
{
    int i;
    FILE *fp;

    if ((fp = fopen(prjfile, "w")) == NULL)
        return emod_set(TMAN_PRJ_SAVE);

    for (i = 0; i < NPRJ && prjs[i][0] != '\0'; ++i)
        fprintf(fp, PRJFMT, prjs[i]);
    return fclose(fp);
}

int project_init(char *fstate)
{
    prjfile = fstate;

    assert(fstate != NULL && "prj state file not passed or NULL");
    return load();
}

int is_project_valid(char *prj)
{
    if (!isalnum(*prj++))
        return 0;
    for (; *prj; ++prj)
        if (!(isalnum(*prj) || *prj == '_' || *prj == '-'))
            return 0;
    return isalnum(*--prj);
}

int project_exist(char *prj)
{
    return ISDIR(genpath_prj(prj));
}

char *project_getcurr()
{
    if (is_project_set(prjs[CPRJ]) == FALSE)
        return NULL;
    return strncpy(curr, prjs[CPRJ], PRJSIZ);
}

char *project_getprev()
{
    if (is_project_set(prjs[PPRJ]) == FALSE)
        return NULL;
    return strncpy(prev, prjs[PPRJ], PRJSIZ);
}

int project_addcurr(char *prj)
{
    /* Prevent duplicates in toggles.  */
    if (is_project_curr(prj) == TRUE)
        return 0;
    strncpy(prjs[PPRJ], prjs[CPRJ], PRJSIZ);
    strncpy(prjs[CPRJ], prj, PRJSIZ);
    return save();
}

int project_delcurr(void)
{
    strncpy(prjs[CPRJ], prjs[PPRJ], PRJSIZ);
    memset(prjs[PPRJ], 0, PRJSIZ);
    return save();
}

int project_delprev(void)
{
    memset(prjs[PPRJ], 0, PRJSIZ);
    return save();
}

int project_swap(void)
{
    char tmp[PRJSIZ + 1];

    if (is_project_set(prjs[PPRJ]) == FALSE
        || is_project_set(prjs[CPRJ]) == FALSE)
        return 1;
    strncpy(tmp, prjs[CPRJ], PRJSIZ);
    strncpy(prjs[CPRJ], prjs[PPRJ], PRJSIZ);
    strncpy(prjs[PPRJ], tmp, PRJSIZ);
    return save();
}

int is_project_curr(char *prj)
{
    if (prj == NULL)
        return FALSE;
    return strncmp(prj, prjs[CPRJ], PRJSIZ) == 0;
}

int is_project_prev(char *prj)
{
    if (prj == NULL)
        return FALSE;
    return strncmp(prj, prjs[PPRJ], PRJSIZ) == 0;
}

int project_valid_length(char *prj)
{
    return strlen(prj) <= PRJSIZ;
}
