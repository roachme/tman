#include <stdio.h>
#include <dirent.h>
#include <string.h>

#include "state.h"
#include "tman.h"
#include "common.h"
#include "unit.h"

#define MARKDEF  "blog"
#define MARKCURR "curr"
#define MARKPREV "prev"



struct state state[NENV];
const char *marks[2] = { "curr", "prev" }; /* syntactic sugar */


static int load_specids(char *type, char *env)
{
    DIR *dp;
    FILE *fp;
    char line[BUFSIZ];
    char funit[1000];
    char denv[500];
    struct dirent *dir;

    sprintf(denv, "%s/%s", tmanfs.task, env);

    if ((dp = opendir(denv)) == NULL) {
        fprintf(stderr, "could not open env dir %s\n", denv);
        return 0;
    }

    while ((dir = readdir(dp)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
            continue;

        sprintf(funit, "%s/%s/.tman/unit", denv, dir->d_name);
        fp = fopen(funit, "r");
        if (!fp) {
            fprintf(stderr, "could not read unit file %s\n", funit);
            continue;
        }

        char key[100], val[100];
        while (fgets(line, BUFSIZ, fp) != NULL) {
            sscanf(line, "%s : %s", key, val);
            if (strcmp(key, "col") == 0) {
                if (strcmp(val, "curr") == 0) {
                    if (strcmp(type, "curr") == 0)
                        strcpy(state[CENV].curr, dir->d_name);
                    else
                        strcpy(state[PENV].curr, dir->d_name);
                }
                else if (strcmp(val, "prev") == 0) {
                    if (strcmp(type, "curr") == 0)
                        strcpy(state[CENV].prev, dir->d_name);
                    else
                        strcpy(state[PENV].prev, dir->d_name);
                }
                break;
            }
        }
        fclose(fp);
    }
    return 0;
}


static int load_envs(const char *fname)
{
    char line[BUFSIZ];
    FILE *fp = fopen(fname, "r");

    if (fp == NULL)
        return 1;

    for (int i = 0; fgets(line, BUFSIZ, fp) != NULL && i < NENV; ++i) {
        struct state *st = &state[i];
        sscanf(line, "%*s %s", st->name);
        //sscanf(line, "%*s %s %s %s", st->name, st->curr, st->prev);
    }
    return fclose(fp);
}

static int show(void)
{
    for (int i = 0; i < NENV; ++i) {
        struct state *st = &state[i];
        fprintf(stderr, "show: '%s %s %s %s'\n", marks[i], st->name, st->curr, st->prev);
    }
    return 0;
}

static int save(const char *fname)
{
    FILE *fp = fopen(fname, "w");

    if (fp == NULL) {
        fprintf(stderr, "err: save\n");
        return 1;
    }
    for (int i = 0; i < NENV; ++i) {
        struct state *st = &state[i];
        fprintf(fp, "%s %s\n", marks[i], st->name);
        //fprintf(fp, "%s %s %s %s\n", marks[i], st->name, st->curr, st->prev);
    }
    return fclose(fp);
}

int state_init()
{
    load_envs(tmanfs.fstate);

    if (strlen(state[CENV].name) > 0)
        load_specids("curr", state[CENV].name);
    if (strlen(state[PENV].name) > 0)
        load_specids("prev", state[PENV].name);
    //show();
    return 0;
}

int state_addcid(char *id)
{
    if (strlen(state[CENV].name) == 0) {
        fprintf(stderr, "state_addcid: no current env set\n");
        return 1;
    } else if (strcmp(state[CENV].curr, id) == 0) {
        fprintf(stderr, "state_addcid: id '%s' already marked as curr\n", id);
        return 1;
    }
    if (strlen(state[CENV].prev) > 0) {
        unit_set2(state[CENV].name, state[CENV].prev, "col", MARKDEF);
        // roach: Unnecessary thingy
        state[CENV].prev[0] = '\0';
    }

    strcpy(state[CENV].prev, state[CENV].curr);
    unit_set2(state[CENV].name, state[CENV].prev, "col", MARKPREV);

    strcpy(state[CENV].curr, id);
    return unit_set2(state[CENV].name, id, "col", MARKCURR);
}

int state_delcid(void)
{
    if (strlen(state[CENV].name) == 0) {
        fprintf(stderr, "state_addcid: no current env set\n");
        return 1;
    }
    if (strlen(state[CENV].prev) > 0) {
        unit_set2(state[CENV].name, state[CENV].prev, "col", MARKDEF);
        // roach: Unnecessary thingy
        state[CENV].prev[0] = '\0';
    }
    if (strlen(state[CENV].curr) > 0) {
        unit_set2(state[CENV].name, state[CENV].curr, "col", MARKDEF);
        // roach: Unnecessary thingy
        state[CENV].curr[0] = '\0';
    }
    strcpy(state[CENV].curr, state[CENV].prev);
    return unit_set2(state[CENV].name, state[CENV].curr, "col", MARKPREV);
}

int state_delpid(void)
{
    if (strlen(state[CENV].name) == 0) {
        fprintf(stderr, "state_addcid: no current env set\n");
        return 1;
    }
    if (strlen(state[CENV].prev) > 0) {
        unit_set2(state[CENV].name, state[CENV].prev, "col", MARKDEF);
        // roach: Unnecessary thingy
        state[CENV].prev[0] = '\0';
    }
    return 0;
}

int state_swapids(void)
{
    char tmp[20];

    if (strlen(state[CENV].name) == 0) {
        fprintf(stderr, "state_addcid: no current env set\n");
        return 1;
    } else if (state[CENV].curr[0] == '\0' || state[CENV].prev[0] == '\0') {
        fprintf(stderr, "state_addcid: no current or previous id set\n");
        return 1;
    }
    strcpy(tmp, state[CENV].curr);

    strcpy(state[CENV].curr, state[CENV].prev);
    unit_set2(state[CENV].name, state[CENV].curr, "col", MARKCURR);

    strcpy(state[CENV].prev, tmp);
    unit_set2(state[CENV].name, state[CENV].prev, "col", MARKPREV);
    return 0;
}

int state_env_add(char *env)
{
    if (strcmp(state[CENV].name, env) == 0)
        return 1;
    state[PENV] = state[CENV];
    strcpy(state[CENV].name, env);
    state[CENV].curr[0] = state[CENV].prev[0] = '\0';
    return save(tmanfs.fstate);
}

int state_env_del(void)
{
    state[CENV] = state[PENV];
    memset(&state[PENV], 0, sizeof(struct state));
    return save(tmanfs.fstate);
}

int state_env_swap(void)
{
    struct state tmp;

    if (state[CENV].name[0] == '\0' || state[PENV].name[0] == '\0')
        return 1;
    tmp = state[CENV];
    state[CENV] = state[PENV];
    state[PENV] = tmp;
    return save(tmanfs.fstate);
}

char *state_getcid()
{
    return state[CENV].curr;
}

char *state_getpid()
{
    return state[CENV].prev;
}

char *state_getcenv()
{
    return state[CENV].name;
}

char *state_getpenv()
{
    return state[PENV].name;
}
