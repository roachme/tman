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

enum tman_errno {
    TMAN_OK = 0,        /* OK, interpret output as path or output */
    /* DELETE this one, cuz it's not part of core return codes */
    TMAN_INVOPT = 1,    /* Error, invalid option */
    TMAN_ECORE  = 2,    /* ?? Error in core execution (under consideration to include) */
    TMAN_EBIN   = 3,    /* Generic: failed to executed command */
    TMAN_EHOOK  = 4,    /* Failed to execute hook */
    TMAN_EPGN   = 5,    /* Failed to execute plugin */
};

#define TRUE            1
#define FALSE           0

#define IDSIZ           20
#define TAGSIZ          4
#define PATHSIZ         4096

int _chkid(char *id);
int _chkenv(char *env);
char *formpath(char *dst, char *fmt, ...);

int elog(int status, char *fmt, ...);
int wlog(char *fmt, ...);
int ilog(char *fmt, ...);
int dlog(char *fmt, ...);

#endif
