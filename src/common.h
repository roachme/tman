#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>

#include "limit.h"

#define PROGRAM "tman"
#define VERSION "v0.1.0"

#define IDSIZ           20
#define CMDSIZ          5
#define PGNSIZ          10
#define ENVSIZ          20
#define TIDSIZ          20
#define PATHSIZ         4096
#define HELPDESC        100


// TODO: move to config file, not a hard-coded value
#define HOME            "/home/roach"
#define TMANBASE        HOME "/trash/tman"
#define TMANPBASE       HOME "/workspace/toolkit/tman"

// Not these ones tho
#define TMANDB          TMANBASE "/.tman"
#define TMANPGNS        TMANBASE "/pgns"
#define TMANTASKS       TMANBASE "/tasks"
#define TMANSTATE       TMANBASE "/.tman/state"
#define TMANINITED      TMANBASE "/.tman/inited"


int elog(char *fmt, ...);
int wlog(char *fmt, ...);
int ilog(char *fmt, ...);
int dlog(char *fmt, ...);

#endif
