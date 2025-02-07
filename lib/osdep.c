#include <stdio.h>
#include <stdlib.h>

#include "osdep.h"

#ifdef __linux__

BOOL MKDIR(char *path)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "mkdir -p %s", path);
    return system(cmd);
}

BOOL RMDIR(char *path)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "rm -rf %s", path);
    return system(cmd);
}

BOOL MOVE(char *src, char *dst)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "mv %s %s", src, dst);
    return system(cmd);
}

BOOL TOUCH(char *path)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "touch %s", path);
    return system(cmd);
}

BOOL ISFILE(char *fname)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "test -f %s", fname);
    return system(cmd) == EXIT_SUCCESS;
}

BOOL ISDIR(char *fname)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "test -d %s", fname);
    return system(cmd) == EXIT_SUCCESS;
}

#elif __APPLE__

BOOL MKDIR(char *path)
{
    return 0;
}

BOOL RMDIR(char *path)
{
    return 0;
}

BOOL MOVE(char *src, char *dst)
{
    return 0;
}

BOOL TOUCH(char *path)
{
    return 0;
}

BOOL ISFILE(char *fname)
{
    return 0;
}

BOOL ISDIR(char *fname)
{
    return 0;
}

#elif WIN32 || __MINGW32__

BOOL MKDIR(char *path)
{
    return 0;
}

BOOL RMDIR(char *path)
{
    return 0;
}

BOOL MOVE(char *src, char *dst)
{
    return 0;
}

BOOL TOUCH(char *path)
{
    return 0;
}

BOOL ISFILE(char *fname)
{
    return 0;
}

BOOL ISDIR(char *fname)
{
    return 0;
}

#endif
