#include "dir.h"
#include "common.h"
#include "osdep.h"

static int _id_dbmkdir(char *base, char *prj, char *id)
{
    char path[PATHSIZ + 1];
    sprintf(path, "%s/%s/%s/.tman", base, prj, id);
    return MKDIR(path);
}

static int _prj_dbmkdir(char *base, char *prj)
{
    char path[PATHSIZ + 1];
    sprintf(path, "%s/%s/.tman", base, prj);
    return MKDIR(path);
}

int dir_id_add(char *base, char *prj, char *id)
{
    char path[PATHSIZ + 1];
    sprintf(path, "%s/%s/%s", base, prj, id);
    return !(MKDIR(path) == 0 && _id_dbmkdir(base, prj, id) == 0);
}

int dir_id_del(char *base, char *prj, char *id)
{
    char path[BUFSIZ + 1];
    sprintf(path, "%s/%s/%s", base, prj, id);
    return RMDIR(path);
}

int dir_id_rename(char *base, char *srcprj, char *dstprj, char *srcid,
                  char *dstid)
{
    char new_path[PATHSIZ + 1];
    char old_path[PATHSIZ + 1];
    sprintf(old_path, "%s/%s/%s", base, srcprj, srcid);
    sprintf(new_path, "%s/%s/%s", base, dstprj, dstid);
    return rename(old_path, new_path);
}

int dir_prj_add(char *base, char *prj)
{
    char path[PATHSIZ + 1];
    sprintf(path, "%s/%s/", base, prj);
    return !(MKDIR(path) == 0 && _prj_dbmkdir(base, prj) == 0);
}

int dir_prj_del(char *base, char *prj)
{
    char path[PATHSIZ + 1];
    sprintf(path, "%s/%s", base, prj);
    return RMDIR(path);
}

int dir_prj_rename(char *base, char *src, char *dst)
{
    char srcpath[PATHSIZ + 1];
    char dstpath[PATHSIZ + 1];

    sprintf(srcpath, "%s/%s", base, src);
    sprintf(dstpath, "%s/%s", base, dst);
    return MOVE(srcpath, dstpath);
}
