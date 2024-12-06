#ifndef TMAN_ENV_H
#define TMAN_ENV_H

#define CENV        0
#define PENV        1  /* index of current env */
#define NENV        2  /* number of 'special' environments */
#define ENVSIZ      20 /* maximum size of environment name */
#define NENVITEM    1  /* number of environment items per line */

int env_init(char *fstate);
char *env_getcurr();
char *env_getprev();
int env_delcenv(void);
int env_delpenv(void);
int env_swapenvs(void);
int env_addcenv(char *env);

#endif
