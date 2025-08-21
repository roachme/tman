#ifndef LIBTEC_DIR_H
#define LIBTEC_DIR_H

#include "libtec.h"

int dir_project_add(const char *taskdir, tec_arg_t * args);
int dir_project_del(const char *taskdir, tec_arg_t * args);
int dir_project_rename(const char *taskdir, tec_arg_t * src, tec_arg_t * dst);

int dir_board_add(const char *taskdir, tec_arg_t * args);
int dir_board_del(const char *taskdir, tec_arg_t * args);
int dir_board_move(const char *taskdir, tec_arg_t * src, tec_arg_t * dst);

int dir_task_add(const char *taskdir, tec_arg_t * args);
int dir_task_del(const char *taskdir, tec_arg_t * args);
int dir_task_move(const char *taskdir, tec_arg_t * src, tec_arg_t * dst);

#endif
