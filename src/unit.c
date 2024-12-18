/*
    Module to operate on task unit.
*/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "tman.h"
#include "unit.h"
#include "common.h"

static struct bunit unit;

/* List of builtin task units */
static char *keys[MAXUBIN] = {
    "id",       /* task ID */
    "prio",     /* task priority */
    "type",     /* task type: bugfix, hotfix, feature */
    "date",     /* task date of creation */
    "desc",     /* task description */
};
static int keynum = sizeof(keys) / sizeof(keys[0]);

static int _chkprio(char *prio)
{
    char *prios[] = { "lowest", "low", "mid", "high", "highest" };
    int size = sizeof(prios) / sizeof(prios[0]);

    for (int i = 0; i < size; ++i) {
        if (strncmp(prio, prios[i], 10) == 0)
            return 1;
    }
    elog(1, "invalid priority '%s'. Try help for more info", prio);
    return 0;
}

static int _chktype(char *type)
{
    char *types[] = { "bugfix", "feature", "hotfix" };
    int size = sizeof(types) / sizeof(types[0]);

    for (int i = 0; i < size; ++i) {
        if (strncmp(type, types[i], 10) == 0)
            return 1;
    }
    elog(1, "invalid type '%s'. Try help for more info", type);
    return 0;
}

static int _chkdesc(char *desc)
{
    if (!isalnum(*++desc))
        return 0;
    for (; *desc; ++desc)
        if (!(isalnum(*desc) || isspace(*desc) || *desc == '_' || *desc == '-'))
            return 0;
    return 1;
}

/*
static int _chkcol(char *col)
{
    const char *cols[] = { "curr", "prev", "blog", "revw", "test", "done" };
    const int ncol = sizeof(cols) / sizeof(cols[0]);

    if (strcmp(col, "curr") == 0 || strcmp(col, "prev") == 0)
        return elog(1, "curr & prev are system columns. Can't change 'em");

    for (int i = 0; i < ncol; ++i)
        if (strcmp(cols[i], col) == 0)
            return 1;
    return 0;
}
*/

/*
 * return 0 - failed
 * return 1 - ok
*/
static int check(char *key, char *val)
{
    if (!strcmp(key, "id"))
        return  _chkid(val);
    else if (!strcmp(key, "prio"))
        return _chkprio(val);
    else if (!strcmp(key, "type"))
        return _chktype(val);
    else if (!strcmp(key, "desc"))
        return _chkdesc(val);
    elog(1, "not found '%s': no such builtin key", key);
    return 0;
}

static int genunit(char *env, char *id)
{
    int i = 0;
    char buff[BUFSIZ + 1];
    time_t rawtime = time(NULL);
    struct tm *timeinfo = localtime(&rawtime);

    // Get current time according to format
    strftime(buff, BUFSIZ, "%Y%m%d", timeinfo);

    strcpy(unit.pair[i].key, "id");
    strcpy(unit.pair[i].val, id);

    ++i;
    strcpy(unit.pair[i].key, "prio");
    strcpy(unit.pair[i].val, "mid");

    ++i;
    strcpy(unit.pair[i].key, "type");
    strcpy(unit.pair[i].val, "hotfix");

    ++i;
    strcpy(unit.pair[i].key, "date");
    strcpy(unit.pair[i].val, buff);

    ++i;
    strcpy(unit.pair[i].key, "desc");
    strcpy(unit.pair[i].val, "generated description for ");
    strcat(unit.pair[i].val, id);
    return 0;
}

static int _load(char *fname, struct bunit *unt)
{
    int i;
    char line[BUFSIZ + 1];
    FILE *fp = fopen(fname, "r");
    unt = unt ? unt : &unit;

    if (!fp)
        return elog(1, "could not open file %s", fname);

    // HOTFIX: prevents duplicates from happening
    memset(unt, 0, sizeof(struct bunit));
    for (i = 0; i < MAXUBIN && fgets(line, BUFSIZ, fp) != NULL; ++i) {
        unt->pair[i].isset = 1;
        sscanf(line, "%s : %[^\n]s", unt->pair[i].key, unt->pair[i].val);
    }

    unt->size = i;
    return fclose(fp);
}

static int _save(char *fname)
{
    FILE *fp = fopen(fname, "w");

    if (!fp)
        return elog(1, "could not open file %s", fname);

    for (int i = 0; i < MAXUBIN; ++i)
        fprintf(fp, "%s : %s\n", unit.pair[i].key, unit.pair[i].val);
    return fclose(fp);
}

int unit_add(char *env, char *id)
{
    char fname[PATHSIZ + 1];
    sprintf(fname, "%s/%s/%s/.tman/unit", tmanfs.base, env, id);

    genunit(env, id);
    return _save(fname);
}

int unit_del(char *env, char *id)
{
    return 0;
}

struct bunit *unit_get(struct bunit *u, char *env, char *id)
{
    char fname[PATHSIZ + 1];
    sprintf(fname, "%s/%s/%s/.tman/unit", tmanfs.base, env, id);

    if (_load(fname, u)) {
        elog(1, "_load: failed");
        return NULL;
    }

    if (u) {
        // TODO: Add task ID into units cuz it won't be there
        // in future releases.
    }
    else {
        elog(1, "unit_get: bunit is NULL");
    }

    return u;
}

/*
   Unit changes are atomic, i.e. if a single option fails check
   none of 'em will set.
*/
int unit_set(char *env, char *id, struct bunit *bunit)
{
    char fname[PATHSIZ + 1];
    sprintf(fname, "%s/%s/%s/.tman/unit", tmanfs.base, env, id);

    if (_load(fname, NULL))
        return 1;

    for (int i = 0; i < unit.size; ++i) {
        // TODO: why is here yet another if statetment??
        if (bunit->pair[i].isset && strcmp(bunit->pair[i].key, unit.pair[i].key) == 0) {
            if (!check(bunit->pair[i].key, bunit->pair[i].val)) {
                fprintf(stderr, "unit_set: error\n");
                return 1;
            }
            strcpy(unit.pair[i].val, bunit->pair[i].val);
        }
    }

    return _save(fname);
}

int unit_set2(char *env, char *id, char *key, char *val)
{
    // TODO: WTF is 1000?
    char funit[1000];
    sprintf(funit, "%s/%s/%s/.tman/unit", tmanfs.base, env, id);
    FILE *fp;

    if (_load(funit, NULL)) {
        fprintf(stderr, "could not load unit file\n");
        return 1;
    }
    if ((fp = fopen(funit, "w")) == NULL) {
        fprintf(stderr, "could not open unit file\n");
        return 1;
    }

    for (int i = 0; i < unit.size; ++i) {
        if (strcmp(unit.pair[i].key, key) == 0)
            strcpy(unit.pair[i].val, val);
        fprintf(fp, "%s : %s\n", unit.pair[i].key, unit.pair[i].val);
    }
    return fclose(fp);
}
