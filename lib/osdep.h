#ifndef OSDEP_H
#define OSDEP_H

#include "common.h"

BOOL MKDIR(char *path);
BOOL RMDIR(char *path);
BOOL MOVE(char *src, char *dst);
BOOL TOUCH(char *path);
BOOL ISFILE(char *fname);
BOOL ISDIR(char *fname);

#endif
