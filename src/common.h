#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

#define PROGRAM "tman-c"
#define VERSION "v0.1.0"

#define IDSIZ           20
#define CMDSIZ          5
#define PGNSIZ          10
#define ENVSIZ          20
#define TIDSIZ          20
#define PATHSIZ         4096
#define HELPDESC        100



// TODO: move to config file, not a hard-coded value
#define TMANBASE        "/home/roach/trash/tman"
#define TMANDB          "/home/roach/trash/tman/.tman"
#define TMANPGNS        "/home/roach/trash/tman/pgns"
#define TMANTASKS       "/home/roach/trash/tman/tasks"
#define TMANSTATE       "/home/roach/trash/tman/.tman/state"
#define TMANINITED      "/home/roach/trash/tman/.tman/inited"

#define TMANPBASE       "/home/roach/workspace/sewer/sketch/c"
#define TMAN_PGNDIR     "/home/roach/workspace/sewer/sketch/c"

int elog(char *fmt, ...);
int wlog(char *fmt, ...);
int ilog(char *fmt, ...);
int dlog(char *fmt, ...);

#endif
