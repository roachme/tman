#ifndef LIBTMAN_TASK_H
#define LIBTMAN_TASK_H

char *task_curr(char *prj);
char *task_prev(char *prj);
int task_is_valid_name(char *id);
int task_is_valid_length(char *id);
int task_add(char *prj, char *id);
int task_exist(char *prj, char *id);
int task_del(char *prj, char *id);
int task_iscurr(char *prj, char *id);
int task_isprev(char *prj, char *id);
int task_move(char *prj, char *id, char *col);
int task_swap(char *prj);

#endif
