#ifndef TMAN_ENV_H
#define TMAN_ENV_H

#define CENV        0  /* index of current env */
#define PENV        1  /* index of previous env */
#define NENV        2  /* number of 'special' environments */
#define ENVSIZ      20 /* maximum size of environment name */
#define NENVITEM    1  /* number of environment items per line */

int env_init(char *fstate);
int env_reset(void);
char *env_getcurr();
char *env_getprev();
int env_delcurr(void);
int env_delprev(void);
int env_swapenvs(void);
int env_addcurr(char *env);
int env_exists(char *env);
int env_isvalid(char *env);

#endif
