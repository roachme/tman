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

extern struct tmanstruct tmanfs;

enum tman_errno {
    TMAN_OK = 0,        /* OK, switch to task ID */
    TMAN_NOPATH = 1,    /* OK, but output should not be treated as path */
    TMAN_INVOPT = 2,    /* Error, invalid option */
    TMAN_ECORE  = 3,    /* ?? Error in core execution (under consideration to include) */
    TMAN_EBIN   = 4,    /* Generic: failed to executed command */
    TMAN_EHOOK  = 5,    /* Failed to execute hook */
    TMAN_EPGN   = 6,    /* Failed to execute plugin */
};

#define TRUE            1
#define FALSE           0

#define IDSIZ           20
#define TAGSIZ          4
#define CMDSIZ          5
#define PGNSIZ          10
#define TIDSIZ          20
#define PATHSIZ         4096
#define HELPDESC        100


int _chkid(char *id);
int _chkenv(char *env);

int elog(int status, char *fmt, ...);
int wlog(char *fmt, ...);
int ilog(char *fmt, ...);
int dlog(char *fmt, ...);

#endif
