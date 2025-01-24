/*
    Module to operate on task unit.
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "unit.h"
#include "common.h"

static struct unit unitbin[NKEYS];
static char *keys[NKEYS] = {
    "prio",     /* task priority */
    "type",     /* task type: bugfix, hotfix, feature */
    "date",     /* task date of creation */
    "desc",     /* task description */
};

static int valid_prio(const char *val);
static int valid_type(const char *val);
static int valid_date(const char *val);
static int valid_desc(const char *val);

struct validval {
    char key[KEYSIZ + 1];
    int (*func) (const char *val);
};

struct validval validval[NKEYS] = {
    { .key = "prio", .func = valid_prio, },
    { .key = "type", .func = valid_type, },
    { .key = "date", .func = valid_date, },
    { .key = "desc", .func = valid_desc, },
};

/* roachme: replace all prios if user specifies any in config file */
static int valid_prio(const char *val)
{
    char *prios[] = { "lowest", "low", "mid", "high", "highest" };
    int size = sizeof(prios) / sizeof(prios[0]);

    for (int i = 0; i < size; ++i)
        if (strncmp(val, prios[i], 10) == 0)
            return TRUE;
    return FALSE;
}

/* roachme: replace all types if user specifies any in config file */
static int valid_type(const char *val)
{
    char *types[] = { "task", "bugfix", "feature", "hotfix" };
    int size = sizeof(types) / sizeof(types[0]);

    for (int i = 0; i < size; ++i) {
        if (strncmp(val, types[i], 10) == 0)
            return TRUE;
    }
    return FALSE;
}

static int valid_date(const char *val)
{
    return TRUE;
}

static int valid_desc(const char *val)
{
    if (!isalnum(*val++))
        return FALSE;
    for ( ; *val; ++val)
        if (!(isalnum(*val) || isspace(*val) || *val == '_' || *val == '-'))
            return FALSE;
    return isalnum(*--val) != 0;
}

static int val_valid(const char *key, const char *val)
{
    int i;

    for (i = 0; i < NKEYS; ++i)
        if (strncmp(validval[i].key, key, KEYSIZ) == 0)
            return validval[i].func(val);
    return FALSE;
}

static int reset_units(void)
{
    memset(unitbin, 0, sizeof(unitbin));
    return 0;
}

static int genitem(int idx, char *key, char *val)
{
    strncpy(unitbin[idx].key, key, KEYSIZ);
    strncpy(unitbin[idx].val, val, VALSIZ);
    return 0;
}

static int setitem(char *key, char *val)
{
    int i;

    for (i = 0; i < NKEYS; ++i)
        if (strncmp(keys[i], key, KEYSIZ) == 0) {
            strncpy(unitbin[i].val, val, VALSIZ);
            return 0;
        }
    return 1;
}

static int key_exists(const char *key)
{
    int i;

    for (i = 0; i < NKEYS; ++i)
        if (strncmp(keys[i], key, KEYSIZ) == 0)
            return TRUE;
    return FALSE;
}

static int chkitem(const char *key, const char *val)
{
    if (key_exists(key) == FALSE)
        return FALSE;
    else if (val_valid(key, val) == FALSE)
        return FALSE;
    return TRUE;
}

/*
 * return 1 (true)  - OK
 * return 0 (fales) - ERR
*/
static int load(const char *fname)
{
    FILE *fp;
    int i, retcode;

    if ((fp = fopen(fname, "r")) == NULL)
        return 1;
    for (i = 0; i < NKEYS; ++i) {
        fscanf(fp, "%s : %[^\n]s", unitbin[i].key, unitbin[i].val);
        if ((retcode = strcmp(keys[i], unitbin[i].key)) != 0)
            break;
        unitbin[i].isset = TRUE;
    }
    fclose(fp);
    return retcode == 0 ? retcode : 1;
}

static int save(const char *fname)
{
    int i;
    FILE *fp;

    if ((fp = fopen(fname, "w")) == NULL)
        return 1;

    for (i = 0; i < NKEYS; ++i)
        fprintf(fp, "%s : %s\n", unitbin[i].key, unitbin[i].val);
    return fclose(fp);
}

static int genitems(char *env, char *id)
{
    int idx = 0;
    char buff[BUFSIZ + 1];
    time_t rawtime = time(NULL);
    const char timefmt[] = "%Y%m%d";
    struct tm *timeinfo = localtime(&rawtime);
    char desc[VALSIZ + 1] = "generated description for task ";

    strftime(buff, BUFSIZ, timefmt, timeinfo);
    genitem(idx++, "prio", "mid");
    genitem(idx++, "type", "hotfix");
    genitem(idx++, "date", buff);
    genitem(idx++, "desc", strncat(desc, id, IDSIZ));
    return save(genpath_unit(env, id));
}

int unit_chkbin(struct unit *units)
{
    int i;

    for (i = 0; i < NKEYS; ++i)
        if (units[i].isset && chkitem(units[i].key, units[i].val) == FALSE)
            return FALSE;
    return TRUE;
}

int unit_addbin(char *env, char *id, struct unit *units)
{
    if (genitems(env, id))
        return 1;
    return unit_setbin(env, id, units);
}

int unit_setbin(char *env, char *id, struct unit *units)
{
    int i;

    if (load(genpath_unit(env, id)))
        return 1;

    for (i = 0; i < NKEYS; ++i)
        setitem(units[i].key, units[i].val);
    return save(genpath_unit(env, id));
}

struct unit *unit_getbin(struct unit *units, char *env, char *id)
{
    int i;

    if (load(genpath_unit(env, id)))
        return NULL;
    for (i = 0; i < NKEYS; ++i)
        units[i] = unitbin[i];
    return units;
}

int unit_delbin(char *env, char *id)
{
    reset_units();
    return save(genpath_unit(env, id));
}

struct unit *unit_addpgn(struct unit *head, char *key, char *val)
{
    struct unit *node;

    if ((node = malloc(sizeof(struct unit))) == NULL)
        return head;

    node->next = head;
    node->isset = TRUE;
    strncpy(node->key, key, KEYSIZ);
    strncpy(node->val, val, VALSIZ);
    return node;
}

int unit_delpgn(struct unit *pgn)
{
    struct unit *tmp;

    while (pgn) {
        tmp = pgn;
        pgn = pgn->next;
        free(tmp);
    }
    return 0;
}
