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


struct unit {
    int prio;  /* in case I'ma use tree data structure */
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    struct unit *next;
};

int unit_add(char *env, char *id, struct unit *units);
struct unit *unit_get(char *env, char *id);
int unit_set(char *env, char *id, struct unit *units);
int unit_del(char *env, char *id);
void unit_free(struct unit *units);

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
    strncpy(unit->key, key, KEYSIZ);
    strncpy(unit->val, val, VALSIZ);

    unit->next = p;
    p = unit;
    return p;
}

static struct unit *getunits(char *fname)
{
    FILE *fp;
    struct unit *units;
    char line[LINESIZ];
    char key[KEYSIZ + 1], val[KEYSIZ + 1];

    if ((fp = fopen(fname, "r")) == NULL)
        return NULL;

    /* TODO: add a checker */
    units = NULL;
    while (fgets(line, LINESIZ, fp) != NULL) {
        sscanf(line, FMTUNIT, key, val);
        units = list_add(units, key, val);
    }
    fclose(fp);
    return units;
}

// roachme: gotta set new values but do not delete old ones
static int setunits(char *fname, struct unit *units)
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
int unit_chk(struct unit *units)
{
    for ( ; units; units = units->next) {
        if (chkkey(units->key) == FALSE)
            return FALSE;
        else if (chkval(units->val) == FALSE)
            return FALSE;
    }
    return TRUE;
}

int unit_add(char *env, char *id, struct unit *units)
{
    return setunits(genpath_unit(env, id), units);
}

struct unit *unit_get(char *env, char *id)
{
    return getunits(genpath_unit(env, id));
}

int unit_del(char *env, char *id)
{
    return setunits(genpath_unit(env, id), NULL);
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

    struct unit *units = unit_get(env, id);
    unit_show(units);
    return 0;
}

