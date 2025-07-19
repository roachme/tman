#ifndef TMAN_TOGGLE_H
#define TMAN_TOGGLE_H

#include "../cli.h"

int toggle_prj_get_curr(char *base, tman_arg_t * args);
int toggle_brd_get_curr(char *base, tman_arg_t * args);
int toggle_task_get_curr(char *base, tman_arg_t * args);

int toggle_task_swap(char *base, tman_arg_t * args);
int toggle_task_get_prev(char *base, tman_arg_t * args);
int toggle_task_set_curr(char *base, tman_arg_t * args);

#endif
