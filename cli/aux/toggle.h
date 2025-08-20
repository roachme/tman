#ifndef TEC_TOGGLE_H
#define TEC_TOGGLE_H

#include "../tec.h"

int toggle_project_swap(char *base, tec_arg_t * args);
int toggle_project_get_curr(char *base, tec_arg_t * args);
int toggle_project_set_curr(char *base, tec_arg_t * args);
int toggle_project_get_prev(char *base, tec_arg_t * args);

int toggle_board_swap(char *base, tec_arg_t * args);
int toggle_board_get_curr(char *base, tec_arg_t * args);
int toggle_board_get_prev(char *base, tec_arg_t * args);
int toggle_board_set_curr(char *base, tec_arg_t * args);

int toggle_task_swap(char *base, tec_arg_t * args);
int toggle_task_get_curr(char *base, tec_arg_t * args);
int toggle_task_get_prev(char *base, tec_arg_t * args);
int toggle_task_set_curr(char *base, tec_arg_t * args);

#endif
