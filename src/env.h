#ifndef TMAN_ENV_H
#define TMAN_ENV_H

#define ENVSIZ      20 /* maximum size of environment name */

int env_init(char *fstate);
int env_reset(void);
char *env_getcurr(void);
char *env_getprev(void);
int env_delcurr(void);
int env_delprev(void);
int env_swapenvs(void);
int env_addcurr(char *env);
int env_exists(char *env);
int env_isvalid(char *env);
int env_iscurr(char *env);
int env_isprev(char *env);

#endif
