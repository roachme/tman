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

int dir_id_rename(char *base, char *srcprj, char *dstprj, char *srcid, char *dstid)
{
    char new_path[PATHSIZ + 1];
    char old_path[PATHSIZ + 1];
    sprintf(old_path, "%s/%s/%s", base, dstprj, srcid);
    sprintf(new_path, "%s/%s/%s", base, srcprj, dstid);
    return rename(old_path, new_path);
}

int dir_id_move(char *base, char *id, char *dstprj, char *srcprj)
{
    char new_path[PATHSIZ + 1];
    char old_path[PATHSIZ + 1];
    sprintf(new_path, "%s/%s/%s", base, dstprj, id);
    sprintf(old_path, "%s/%s/%s", base, srcprj, id);

    if (ISFILE(new_path))
        return 1;
    return MOVE(old_path, new_path);
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
