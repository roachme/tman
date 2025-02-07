#ifndef TMAN_PRJ_H
#define TMAN_PRJ_H

#define PRJSIZ      20 /* maximum size of project name */

int prj_init(char *fstate);
int prj_reset(void);
char *prj_getcurr(void);
char *prj_getprev(void);
int prj_delcurr(void);
int prj_delprev(void);
int prj_swapprjs(void);
int prj_addcurr(char *prj);
int prj_exists(char *prj);
int prj_isvalid(char *prj);
int prj_iscurr(char *prj);
int prj_isprev(char *prj);

#endif
