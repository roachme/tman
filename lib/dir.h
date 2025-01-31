#ifndef DIR_H
#define DIR_H

int dir_id_add(char *base, char *env, char *id);
int dir_id_del(char *base, char *env, char *id);
int dir_id_ren(char *base, char *env, char *newid, char *oldid);
int dir_id_move(char *base, char *id, char *dstenv, char *srcenv);

int dir_env_add(char *base, char *env);
int dir_env_del(char *base, char *env);

#endif
