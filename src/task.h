#ifndef TMAN_TASK_H
#define TMAN_TASK_H

#include "common.h"

#define COLSIZ      10

#ifdef MYTEST
#define IDSIZ       10
#define ENVSIZ      10
#define PATHSIZ     1000
#endif

#define MARKCURR     "curr"
#define MARKPREV     "prev"
#define MARKBLOG     "blog"
#define MARKDONE     "done"

int task_add(char *env, char *id, char *col);
int task_del(char *env, char *id);
int task_exists(char *env, char *id);
char *task_getcurr(char *env);
char *task_getprev(char *env);
int task_move(char *env, char *id, char *col);
int task_swap(char *env);

#endif
