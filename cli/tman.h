#ifndef TMAN_H
#define TMAN_H

#include "../src/common.h"

typedef struct builtin {
    char name[CMDSIZ + 1];
    int setuplvl;
    int (*func)(int argc, char **argv);
} builtin_t;

// roach: gotta either make it clear or get rid of this limitation.
#define TMANPATHSIZE    1024
struct tmanstruct {
    char db[TMANPATHSIZE + 1];          /* directory for tman metadata */
    char cfg[TMANPATHSIZE + 1];         /* directory for config files */
    char pgn[TMANPATHSIZE + 1];         /* directory for plugin data */
    char base[TMANPATHSIZE + 1];        /* directory for all stuff above */
    char pgnins[TMANPATHSIZE + 1];      /* directory for installed plugins */

    char fcfg[TMANPATHSIZE + 1];        /* system config file */
    char finit[TMANPATHSIZE + 1];       /* file that tells that tman is inited */
    char fstate[TMANPATHSIZE + 1];      /* file to store tman task state */
};

int tman_cfg (int argc, char **argv);
int tman_chk (int argc, char **argv);
int tman_init(int argc, char **argv);
int tman_pgm (int argc, char **argv);

int tman_add (int argc, char **argv);
int tman_col (int argc, char **argv);
int tman_del (int argc, char **argv);
int tman_prev(int argc, char **argv);
int tman_set (int argc, char **argv);
int tman_use (int argc, char **argv);

int tman_env (int argc, char **argv);
int tman_move(int argc, char **argv);
int tman_sync(int argc, char **argv);

int tman_cat (int argc, char **argv);
int tman_help(int argc, char **argv);
int tman_list(int argc, char **argv);
int tman_ver (int argc, char **argv);

#endif
