#include <stdio.h>
#include <string.h>

#include "state.h"
#include "common.h"


struct state state[NENV];
const char *marks[2] = { "curr", "prev" }; /* syntactic sugar */

static int load(const char *fname)
{
    char line[BUFSIZ];
    FILE *fp = fopen(fname, "r");

    if (fp == NULL)
        return 1;

    for (int i = 0; fgets(line, BUFSIZ, fp) != NULL && i < NENV; ++i) {
        struct state *st = &state[i];
        sscanf(line, "%*s %s %s %s", st->name, st->curr, st->prev);
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
    char line[BUFSIZ];
    FILE *fp = fopen(fname, "w");

    if (fp == NULL) {
        fprintf(stderr, "err: save\n");
        return 1;
    }
    for (int i = 0; i < NENV; ++i) {
        struct state *st = &state[i];
        fprintf(fp, "%s %s %s %s\n", marks[i], st->name, st->curr, st->prev);
    }
    return fclose(fp);
}

int state_init()
{
    return load(TMANSTATE);
}

int state_id_add(char *id)
{
    if (state[CENV].name[0] == '\0')
        return 1;
    else if (strcmp(state[CENV].curr, id) == 0)
        return 1;
    strcpy(state[CENV].prev, state[CENV].curr);
    strcpy(state[CENV].curr, id);
    return save(TMANSTATE);
}

int state_id_del(void)
{
    if (state[CENV].name[0] == '\0')
        return 1;
    strcpy(state[CENV].curr, state[CENV].prev);
    state[CENV].prev[0] = '\0';
    return save(TMANSTATE);
}

int state_id_pdel(void)
{
    if (state[CENV].name[0] == '\0' || state[CENV].curr[0] == '\0')
        return 1;

    fprintf(stderr, "pdel: curr: %s prev: %s\n", state[CENV].curr, state[CENV].prev);
    state[CENV].prev[0] = '\0';
    return save(TMANSTATE);
}

int state_id_swap(void)
{
    char tmp[SSIZE + 1];

    if (state[CENV].name[0] == '\0')
        return 1;
    else if (state[CENV].curr[0] == '\0' || state[CENV].prev[0] == '\0')
        return 1;
    strcpy(tmp, state[CENV].curr);
    strcpy(state[CENV].curr, state[CENV].prev);
    strcpy(state[CENV].prev, tmp);
    return save(TMANSTATE);
}

int state_env_add(char *env)
{
    if (strcmp(state[CENV].name, env) == 0)
        return 1;
    state[PENV] = state[CENV];
    strcpy(state[CENV].name, env);
    state[CENV].curr[0] = state[CENV].prev[0] = '\0';
    return save(TMANSTATE);
}

int state_env_del(void)
{
    state[CENV] = state[PENV];
    memset(&state[PENV], 0, sizeof(struct state));
    return save(TMANSTATE);
}

int state_env_swap(void)
{
    struct state tmp;

    if (state[CENV].name[0] == '\0' || state[PENV].name[0] == '\0')
        return 1;
    tmp = state[CENV];
    state[CENV] = state[PENV];
    state[PENV] = tmp;
    return save(TMANSTATE);
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
