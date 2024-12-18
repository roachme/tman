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

extern struct tmanstruct tmanfs;

#define TRUE            1
#define FALSE           0

#define IDSIZ           20
#define TAGSIZ          4
#define PATHSIZ         4096

int _chkid(char *id);
int _chkenv(char *env);
char *formpath(char *dst, char *fmt, ...);

int elog(int status, const char *fmt, ...);
int wlog(char *fmt, ...);
int ilog(char *fmt, ...);
int dlog(char *fmt, ...);

#endif
