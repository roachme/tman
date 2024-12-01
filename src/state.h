#ifndef TMAN_STATE_H
#define TMAN_STATE_H

#include "column.h"
#include "common.h"

struct envstate {
    char curr[IDSIZ + 1];
    char prev[IDSIZ + 1];
    char name[ENVSIZ + 1];
};

int state_init(char *fstate, char *db);

/* Getters:BEGIN common id&env */
char *state_getcid();
char *state_getpid();
char *state_getcenv();
char *state_getpenv();
/* Getters:END common id&env */

/* Setters:BEGIN id */
int state_setcid(char *id);
int state_setpid(char *id);
int state_addcid(char *id);
int state_delcid();
int state_delpid();
int state_swapids();
/* Setters:END id */

/* Setters:BEGIN env */
int state_addcenv(char *cenv);
int state_delcenv(void);
int state_delpenv(void);
int state_swapenvs(void);
/* Setters:END env */

#endif
