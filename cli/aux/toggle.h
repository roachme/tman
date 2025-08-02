#ifndef TMAN_TOGGLE_H
#define TMAN_TOGGLE_H

#include "../cli.h"

int toggle_project_swap(char *base, tman_arg_t * args);
int toggle_project_get_curr(char *base, tman_arg_t * args);
int toggle_project_get_prev(char *base, tman_arg_t * args);
int toggle_project_set_curr(char *base, tman_arg_t * args);

int toggle_board_swap(char *base, tman_arg_t * args);
int toggle_board_get_curr(char *base, tman_arg_t * args);
int toggle_board_set_curr(char *base, tman_arg_t * args);
int toggle_board_get_prev(char *base, tman_arg_t * args);

int toggle_task_swap(char *base, tman_arg_t * args);
int toggle_task_get_curr(char *base, tman_arg_t * args);
int toggle_task_set_curr(char *base, tman_arg_t * args);
int toggle_task_get_prev(char *base, tman_arg_t * args);

#endif
