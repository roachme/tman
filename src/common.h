#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <getopt.h>

#include "env.h"
#include "limit.h"

#define PROGRAM     "tman"
#define VERSION     "v0.0.1"
#define TRUE        1
#define FALSE       0

#define DESCSIZ         80
#define PGNOUTSCSIZ     80

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

extern struct tmanstruct tmanfs;

#define BOOL            int
#define TRUE            1
#define FALSE           0

#define IDSIZ           20
#define PATHSIZ         4096

int _chkid(char *id);
int _chkenv(char *env);
char *formpath(char *dst, char *fmt, ...);

char *genpath_env(char *env);
char *genpath_col(char *env, char *id);
char *genpath_full(char *env, char *id);
char *genpath_unit(char *env, char *id);

int elog(int status, const char *fmt, ...);
int wlog(char *fmt, ...);
int ilog(char *fmt, ...);
int dlog(char *fmt, ...);

#endif
