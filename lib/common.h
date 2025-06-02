#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <getopt.h>

#include "project.h"
#include "limit.h"

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

char *genpath_prj(char *prj);
char *genpath_unit_prj(char *prj);
char *genpath_col(char *prj, char *id);
char *genpath_full(char *prj, char *id);
char *genpath_unit(char *prj, char *id);
char *genpath_pgn(char *prj, char *id, char *name, char *cmd);

#endif
