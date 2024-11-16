#include <stdio.h>
#include <stdlib.h>

#include "osdep.h"

int MKDIR(char *path)
{
    char cmd[BUFSIZ];
    sprintf(cmd, "mkdir -p %s", path);
    return system(cmd);
}

int RMDIR(char *path)
{
    char cmd[BUFSIZ];
    sprintf(cmd, "rm -rf %s", path);
    return system(cmd);
}

int MOVE(char *src, char *dst)
{
    char cmd[BUFSIZ];
    sprintf(cmd, "mv %s %s", src, dst);
    return system(cmd);
}

int TOUCH(char *path)
{
    char cmd[BUFSIZ];
    sprintf(cmd, "touch %s", path);
    return system(cmd);
}

int FCHK(char *fname)
{
    char cmd[BUFSIZ];
    sprintf(cmd, "test -f %s", fname);
    return system(cmd);
}

int DCHK(char *fname)
{
    char cmd[BUFSIZ];
    sprintf(cmd, "test -d %s", fname);
    return system(cmd);
}