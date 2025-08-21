#ifndef LIBTEC_OSDEP_H
#define LIBTEC_OSDEP_H

#include <stdbool.h>

bool MKDIR(char *path);
bool RMDIR(char *path);
bool MOVE(char *src, char *dst);
bool TOUCH(char *path);
bool ISFILE(char *fname);
bool ISDIR(char *fname);

#endif
