#ifndef LIBTEC_PATH_H
#define LIBTEC_PATH_H

#include "libtec.h"

char *path_project_dir(const char *taskdir, const tec_arg_t * args);
char *path_board_dir(const char *taskdir, const tec_arg_t * args);
char *path_task_dir(const char *taskdir, const tec_arg_t * args);
char *path_project_board(const char *taskdir, const tec_arg_t * args);

char *path_project_unit(const char *taskdir, const tec_arg_t * args);
char *path_board_unit(const char *taskdir, const tec_arg_t * args);
char *path_task_unit(const char *taskdir, const tec_arg_t * args);

char *path_project_column(const char *taskdir, const tec_arg_t * args);
char *path_board_column(const char *taskdir, const tec_arg_t * args);
char *path_task_column(const char *taskdir, const tec_arg_t * args);

#endif
