#ifndef STATE_H
#define STATE_H

#include "common.h"

#define CENV    0   /* current environment index */
#define PENV    1   /* previous environment index */
#define NENV    2   /* number of environment in state */
#define SSIZE   10  /* size of entry */

struct state {
    char name[SSIZE + 1];
    char curr[SSIZE + 1];
    char prev[SSIZE + 1];
};

int state_init();

char *state_getcid(void);
char *state_getpid(void);
char *state_getcenv(void);
char *state_getpenv(void);

int state_swapids(void);
int state_delpid(void);
int state_delcid(void);
int state_addcid(char *id);

int state_env_del();
int state_env_swap();
int state_env_add(char *env);


struct state_env state_getcurr();

#endif
