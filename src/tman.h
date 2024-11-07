#ifndef TMAN_H
#define TMAN_H

#include "common.h"

typedef struct builtin {
    char name[CMDSIZ + 1];
    int setuplvl;
    int (*func)(int argc, char **argv);
} builtin_t;

int tman_init(int argc, char **argv);
int tman_cfg (int argc, char **argv);
int tman_pgm (int argc, char **argv);

int tman_add (int argc, char **argv);
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
