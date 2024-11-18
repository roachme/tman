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

/* List of builtin task units */
static char *keys[] = {
    "id",       /* task ID */
    "col",      /* task column: under development: should not be outputed directly */
    "prio",     /* task priority */
    "type",     /* task type: bugfix, hotfix, feature */
    "date",     /* task date of creation */
    "desc",     /* task description */
};
static int keynum = sizeof(keys) / sizeof(keys[0]);

/*
    @return 0 - failed
    @return 1 - ok
    else if (!_chkid(id)) - failed

    The isalnum() and isalnum_l() functions shall return non-zero
    if c is an alphanumeric character; otherwise, they shall
    return 0.
*/
int _chkid(char *id)
{
    if (!isalnum(*id++))
        return 0;
    for (; *id; ++id)
        if (!(isalnum(*id) || *id == '_' || *id == '-'))
            return 0;
    return isalnum(*--id);
}

int _chkenv(char *env)
{
    if (!isalnum(*env++))
        return 0;
    for (; *env; ++env)
        if (!(isalnum(*env) || *env == '_' || *env == '-'))
            return 0;
    return isalnum(*--env);
}

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

static struct bunit unit;

int genunit(char *env, char *id)
{
    int i = 0;
    char buff[BUFSIZ];
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
    char line[BUFSIZ];
    FILE *fp = fopen(fname, "r");
    unt = unt ? unt : &unit;

    // HOTFIX: prevents duplicates from happening
    memset(unt, 0, sizeof(struct bunit));

    if (!fp) {
        fprintf(stderr, "could not open file %s\n", fname);
        return 1;
    }

    memset(&unit, 0, sizeof(struct bunit));
    for (int i = 0; fgets(line, BUFSIZ, fp) != NULL; ++i) {
        //printf("line> %s", line);
        ++unt->size;
        unt->pair[i].isset = 1;
        sscanf(line, "%s : %[^\n]s", unt->pair[i].key, unt->pair[i].val);
    }

    return fclose(fp);
}

static int _save(char *fname)
{
    FILE *fp = fopen(fname, "w");

    if (!fp)
        return elog(1, "could not open file %s\n", fname);

    for (int i = 0; i < BINSIZ; ++i) {
        unit.pair[i].isset = 1;
        fprintf(fp, "%s : %s\n", unit.pair[i].key, unit.pair[i].val);
    }

    return fclose(fp);
}

int unit_add(char *env, char *id)
{
    char fname[PATHSIZ + 1];
    sprintf(fname, "%s/%s/%s/.tman/unit", tmanfs.task, env, id);

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
    sprintf(fname, "%s/%s/%s/.tman/unit", tmanfs.task, env, id);

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
    sprintf(fname, "%s/%s/%s/.tman/unit", tmanfs.task, env, id);

    if (_load(fname, NULL))
        return 1;

    for (int i = 0; i < unit.size; ++i) {
        // TODO: why is here yet another if statetment??
        if (bunit->pair[i].isset && strcmp(bunit->pair[i].key, unit.pair[i].key) == 0) {
            if (!check(bunit->pair[i].key, bunit->pair[i].val))
                return 1;
            strcpy(unit.pair[i].val, bunit->pair[i].val);
        }
    }

    return _save(fname);
}

