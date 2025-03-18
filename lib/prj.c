/*
 * Module for 'special' project names - current and previous.
 * Save these project names to be able to switch between them.
 * They also can be used as defaults.
*/

#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "prj.h"
#include "common.h"
#include "errmod.h"
#include "osdep.h"

#define CPRJ        0  /* index of current prj */
#define PPRJ        1  /* index of previous prj */
#define NPRJ        2  /* number of 'special' project */
#define PRJFMT      "%s\n" /* project file format */
#define NPRJITEM    1  /* number of project items per line */

static char *prjfile;
static char prjs[NPRJ][PRJSIZ + 1];

/* Holders for caller functions. This way caller can't mess with module.  */
static char curr[PRJSIZ + 1], prev[PRJSIZ + 1];

static int is_prjset(char *prj)
{
    if (prj == NULL || prj[0] == '\0')
        return FALSE;
    return TRUE;
}

static int load(void)
{
    int i;
    FILE *fp;

    if ((fp  = fopen(prjfile, "r")) == NULL)
        return emod_set(TMAN_PRJ_LOAD);

    for (i = 0; i < NPRJ && fscanf(fp, PRJFMT, prjs[i]) == NPRJITEM; ++i)
        ;
    return fclose(fp);
}

static int save(void)
{
    int i;
    FILE *fp;

    if ((fp  = fopen(prjfile, "w")) == NULL)
        return emod_set(TMAN_PRJ_SAVE);

    for (i = 0; i < NPRJ && prjs[i][0] != '\0'; ++i)
        fprintf(fp, PRJFMT, prjs[i]);
    return fclose(fp);
}

int prj_init(char *fstate)
{
    prjfile = fstate;

    assert(fstate != NULL && "prj state file not passed or NULL");
    return load();
}

int prj_isvalid(char *prj)
{
    if (!isalnum(*prj++))
        return 0;
    for (; *prj; ++prj)
        if (!(isalnum(*prj) || *prj == '_' || *prj == '-'))
            return 0;
    return isalnum(*--prj);
}

int prj_exists(char *prj)
{
    return ISDIR(genpath_prj(prj));
}

int prj_reset(void)
{
    int i;

    for (i = 0; i < NPRJ; ++i)
        memset(prjs[i], 0, PRJSIZ);
    return 0;
}

char *prj_getcurr()
{
    if (is_prjset(prjs[CPRJ]) == FALSE)
        return NULL;
    return strncpy(curr, prjs[CPRJ], PRJSIZ);
}

char *prj_getprev()
{
    if (is_prjset(prjs[PPRJ]) == FALSE)
        return NULL;
    return strncpy(prev, prjs[PPRJ], PRJSIZ);
}

int prj_addcurr(char *prj)
{
    /* Prevent duplicates in toggles.  */
    if (prj_iscurr(prj) == TRUE)
        return 0;
    strncpy(prjs[PPRJ], prjs[CPRJ], PRJSIZ);
    strncpy(prjs[CPRJ], prj, PRJSIZ);
    return save();
}

int prj_delcurr(void)
{
    strncpy(prjs[CPRJ], prjs[PPRJ], PRJSIZ);
    memset(prjs[PPRJ], 0, PRJSIZ);
    return save();
}

int prj_delprev(void)
{
    memset(prjs[PPRJ], 0, PRJSIZ);
    return save();
}

int prj_swapprjs(void)
{
    char tmp[PRJSIZ + 1];

    if (is_prjset(prjs[PPRJ]) == FALSE || is_prjset(prjs[CPRJ]) == FALSE)
        return 1;
    strncpy(tmp, prjs[CPRJ], PRJSIZ);
    strncpy(prjs[CPRJ], prjs[PPRJ], PRJSIZ);
    strncpy(prjs[PPRJ], tmp, PRJSIZ);
    return save();
}

int prj_iscurr(char *prj)
{
    if (prj == NULL)
        return FALSE;
    return strncmp(prj, prjs[CPRJ], PRJSIZ) == 0;
}

int prj_isprev(char *prj)
{
    if (prj == NULL)
        return FALSE;
    return strncmp(prj, prjs[PPRJ], PRJSIZ) == 0;
}

int prj_chklen(char *prj)
{
    return strlen(prj) <= PRJSIZ;
}
