#ifndef TMAN_TASK_H
#define TMAN_TASK_H

char *task_curr(char *env);
char *task_prev(char *env);
int task_chk(char *id);
int task_add(char *env, char *id);
int task_ext(char *env, char *id);
int task_del(char *env, char *id);
int task_move(char *env, char *id, char *col);
int task_swap(char *env);

#endif
