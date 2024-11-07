#ifndef DIR_H
#define DIR_H

int touch(char *pathname);
int imkdir(char *base, char *env, char *id);
int irmdir(char *base, char *env, char *id);
int irndir(char *base, char *env, char *newid, char *oldid);
int imove(char *base, char *id, char *dstenv, char *srcenv);

int emkdir(char *base, char *env);
int ermdir(char *base, char *env);
#endif
