#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE        1
#define FALSE       0

#define FMTUNIT     "%s : %s\n"

#define KEYSIZ      10
#define VALSIZ      40
#define PADDING     10
#define LINESIZ     KEYSIZ + VALSIZ + PADDING
#define PATHSIZ     1000

#define FUNCSIZ     10


struct unit {
    int prio;  /* in case I'ma use tree data structure */
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    struct unit *next;
};

struct chkfuncs {
    char name[FUNCSIZ + 1];
    int (*func) (char *val);
};

static int check_date(char *type);
static int check_desc(char *type);
static int check_prio(char *type);
static int check_type(char *type);

struct chkfuncs chkfuncs[] = {
    { .name = "date", .func = check_date },
    { .name = "desc", .func = check_desc },
    { .name = "prio", .func = check_prio },
    { .name = "type", .func = check_type },
};

int unit_add(char *env, char *id, struct unit *units);
int unit_chkbin(struct unit *units);
int unit_chkpgn(struct unit *units);
struct unit *unit_get(char *env, char *id);
int unit_set(char *env, char *id, struct unit *units);
int unit_del(char *env, char *id);
void unit_free(struct unit *units);

void unit_show(struct unit *units);

static char *base = "/home/roach/trash/tman";
static char pathname[PATHSIZ + 1];

static char *genpath_unit(char *env, char *id)
{
    sprintf(pathname, "%s/%s/%s/.tman/unit", base, env, id);
    return pathname;
}

static int chkkey(char *key)
{
    return TRUE;
}

static int chkval(char *key)
{
    return TRUE;
}

static struct unit *list_add(struct unit *p, char *key, char *val)
{
    struct unit *unit;

    if ((unit = malloc(sizeof(struct unit))) == NULL)
        return p;
    unit->next = p;
    strncpy(unit->key, key, KEYSIZ);
    strncpy(unit->val, val, VALSIZ);
    return unit;
}

static struct unit *list_reverse(struct unit *units)
{
    struct unit *prev, *next;

    for (prev = NULL; units; ) {
        next = units->next;
        units->next = prev;
        prev = units;
        units = next;
    }
    return prev;
}

static struct unit *load_units(char *fname)
{
    FILE *fp;
    struct unit *units;
    char line[LINESIZ];
    char key[KEYSIZ + 1], val[KEYSIZ + 1];

    if ((fp = fopen(fname, "r")) == NULL)
        return NULL;

    /* TODO: add a basic checker, i.e. check for semicolumn.  */
    for (units = NULL; fgets(line, LINESIZ, fp) != NULL; ) {
        sscanf(line, FMTUNIT, key, val);
        units = list_add(units, key, val);
    }
    fclose(fp);
    return list_reverse(units);
}

static int save_units(char *fname, struct unit *units)
{
    FILE *fp;
    struct unit *tmp;

    if ((fp = fopen(fname, "w")) == NULL)
        return 1;

    for (tmp = units; tmp; tmp = tmp->next)
        fprintf(fp, FMTUNIT, tmp->key, tmp->val);
    return fclose(fp);
}

/* Check that units' keys and values are valid */
static int check_units(struct unit *units)
{
    for ( ; units; units = units->next) {
        if (chkkey(units->key) == FALSE)
            return FALSE;
        if (chkval(units->val) == FALSE)
            return FALSE;
    }
    return TRUE;
}

static int check_date(char *type)
{
    return TRUE;
}

static int check_desc(char *type)
{
    return TRUE;
}

static int check_prio(char *prio)
{
    int i;
    /* roach: load 'em from config */
    char *prios[] = { "lowest", "low", "mid", "high", "highest" };

    for (i = 0; i < sizeof(prios) / sizeof(prios[0]); ++i)
        if (strcmp(prio, prios[i]) == 0)
            return TRUE;
    return FALSE;
}

static int check_type(char *type)
{
    int i;
    /* roach: load 'em from config */
    char *types[] = { "feature", "hotfix", "bugfix" };

    for (i = 0; i < sizeof(types) / sizeof(types[0]); ++i)
        if (strcmp(type, types[i]) == 0)
            return TRUE;
    return FALSE;
}

static int check_generic(struct unit *unitbin)
{
    return TRUE;
}

int unit_chkbin(struct unit *units)
{
    if (check_units(units))
        return 1;

    return check_generic(units);
}

int unit_chkpgn(struct unit *units)
{
    return check_units(units);
}

int unit_set(char *env, char *id, struct unit *units)
{
    struct unit *currunits, *pcurrunits, *punits;

    if ((currunits = unit_get(env, id)) == NULL)
        return 1;

    for (punits = units; punits; punits = punits->next) {
        for (pcurrunits = currunits ; pcurrunits; pcurrunits = pcurrunits->next)
            if (strncmp(punits->key, pcurrunits->key, KEYSIZ) == 0)
                strncpy(pcurrunits->val, punits->val, VALSIZ);
    }
    return save_units(genpath_unit(env, id), currunits);
}

int unit_add(char *env, char *id, struct unit *units)
{
    char *fname;

    fname = genpath_unit(env, id);
    return save_units(fname, units);
}

struct unit *unit_get(char *env, char *id)
{
    char *fname;

    fname = genpath_unit(env, id);
    return load_units(fname);
}

int unit_del(char *env, char *id)
{
    char *fname;

    fname = genpath_unit(env, id);
    return save_units(fname, NULL);
}

void unit_free(struct unit *units)
{
    struct unit *tmp;

    while (units != NULL) {
        tmp = units;
        units = units->next;
        free(tmp);
    }
}

void unit_show(struct unit *units)
{
    for ( ; units; units = units->next)
        printf("%-7s : %s\n", units->key, units->val);
}

int main(void)
{
    char *env = "test";
    char *id = "test1";
    struct unit newunits = {
        .key = "fuckme",
        .val = "daddy",
    };

    if (unit_chkbin(&newunits) == FALSE) {
        printf("err: unit check failed\n");
        return 0;
    }

    struct unit *units = unit_get(env, id);
    //unit_set(env, id, &newunits);
    unit_show(units);
    return 0;
}

