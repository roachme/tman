#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <getopt.h>

#include "prj.h"
#include "limit.h"

#define PROGRAM     "tman"
#define VERSION     "v0.0.3"
#define TRUE        1
#define FALSE       0

#define DESCSIZ         80
#define PGNOUTSIZ       80

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define xstr(s) str(s)
#define str(s) #s

extern struct tmanstruct tmanfs;

#define BOOL            int
#define TRUE            1
#define FALSE           0

#define IDSIZ           10
#define PATHSIZ         4096

int _chkid(char *id);
int _chkprj(char *prj);
char *formpath(char *dst, char *fmt, ...);

char *genpath_prj(char *prj);
char *genpath_col(char *prj, char *id);
char *genpath_full(char *prj, char *id);
char *genpath_unit(char *prj, char *id);
char *genpath_pgn(char *prj, char *id, char *name, char *cmd);

int elog(int status, const char *fmt, ...);
int wlog(char *fmt, ...);
int ilog(char *fmt, ...);
int dlog(char *fmt, ...);

#endif
