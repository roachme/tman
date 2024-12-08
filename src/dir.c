#include "dir.h"
#include "common.h"
#include "osdep.h"

static int _dbmkdir(char *base, char *env, char *id) {
  char path[PATHSIZ + 1];
  sprintf(path, "%s/%s/%s/.tman", base, env, id);
  return MKDIR(path);
}

int imkdir(char *base, char *env, char *id) {
  char path[PATHSIZ + 1];
  sprintf(path, "%s/%s/%s", base, env, id);
  return !(MKDIR(path) == 0 && _dbmkdir(base, env, id) == 0);
}

int irmdir(char *base, char *env, char *id) {
  char path[BUFSIZ + 1];
  sprintf(path, "%s/%s/%s", base, env, id);
  return RMDIR(path);
}

int irndir(char *base, char *env, char *newid, char *oldid) {
  char new_path[PATHSIZ + 1];
  char old_path[PATHSIZ + 1];
  sprintf(new_path, "%s/%s/%s", base, env, newid);
  sprintf(old_path, "%s/%s/%s", base, env, newid);
  return rename(old_path, new_path);
}

int imove(char *base, char *id, char *dstenv, char *srcenv) {
  char new_path[PATHSIZ + 1];
  char old_path[PATHSIZ + 1];
  sprintf(new_path, "%s/%s/%s", base, dstenv, id);
  sprintf(old_path, "%s/%s/%s", base, srcenv, id);

  if (ISFILE(new_path))
    return 1;
  return MOVE(old_path, new_path);
}

int emkdir(char *base, char *env) {
  char path[PATHSIZ + 1];
  sprintf(path, "%s/%s/", base, env);
  return MKDIR(path);
}

int ermdir(char *base, char *env) {
  char path[PATHSIZ + 1];
  sprintf(path, "%s/%s", base, env);
  return RMDIR(path);
}
