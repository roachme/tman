#ifndef OSDEP_H
#define OSDEP_H

int MKDIR(char *path);
int RMDIR(char *path);
int MOVE(char *src, char *dst);
int TOUCH(char *path);
int ISFILE(char *fname);
int ISDIR(char *fname);

#endif
