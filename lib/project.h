#ifndef LIBTMAN_PROJECT_H
#define LIBTMAN_PROJECT_H

#define PRJSIZ      10          /* maximum size of project name */

int project_init(char *fstate);
char *project_getcurr(void);
char *project_getprev(void);
int project_delcurr(void);
int project_delprev(void);
int project_swap(void);
int project_addcurr(char *project);
int project_exist(char *project);
int is_project_valid(char *project);
int is_project_curr(char *project);
int is_project_prev(char *project);
int project_valid_length(char *project);

#endif
