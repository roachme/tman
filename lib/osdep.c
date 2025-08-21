#include <stdio.h>
#include <stdlib.h>

#include "osdep.h"

#ifdef __linux__

bool MKDIR(char *path)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "mkdir -p %s", path);
    return system(cmd);
}

bool RMDIR(char *path)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "rm -rf %s", path);
    return system(cmd);
}

bool MOVE(char *src, char *dst)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "mv %s %s", src, dst);
    return system(cmd);
}

bool TOUCH(char *path)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "touch %s", path);
    return system(cmd);
}

bool ISFILE(char *fname)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "test -f %s", fname);
    return system(cmd) == EXIT_SUCCESS;
}

bool ISDIR(char *fname)
{
    char cmd[BUFSIZ + 1];
    sprintf(cmd, "test -d %s", fname);
    return system(cmd) == EXIT_SUCCESS;
}

#elif __APPLE__

bool MKDIR(char *path)
{
    return 0;
}

bool RMDIR(char *path)
{
    return 0;
}

bool MOVE(char *src, char *dst)
{
    return 0;
}

bool TOUCH(char *path)
{
    return 0;
}

bool ISFILE(char *fname)
{
    return 0;
}

bool ISDIR(char *fname)
{
    return 0;
}

#elif WIN32 || __MINGW32__

bool MKDIR(char *path)
{
    return 0;
}

bool RMDIR(char *path)
{
    return 0;
}

bool MOVE(char *src, char *dst)
{
    return 0;
}

bool TOUCH(char *path)
{
    return 0;
}

bool ISFILE(char *fname)
{
    return 0;
}

bool ISDIR(char *fname)
{
    return 0;
}

#endif
