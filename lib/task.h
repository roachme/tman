#ifndef TMAN_TASK_H
#define TMAN_TASK_H

char *task_curr(char *prj);
char *task_prev(char *prj);
int task_chk(char *id);
int task_chklen(char *id);
int task_add(char *prj, char *id);
int task_ext(char *prj, char *id);
int task_del(char *prj, char *id);
int task_iscurr(char *prj, char *id);
int task_move(char *prj, char *id, char *col);
int task_swap(char *prj);

#endif
