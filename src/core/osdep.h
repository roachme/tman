#ifndef OSDEP_H
#define OSDEP_H

int MKDIR(char *path);
int RMDIR(char *path);
int MOVE(char *src, char *dst);
int TOUCH(char *path);
int FCHK(char *fname);
int DCHK(char *fname);

#endif
