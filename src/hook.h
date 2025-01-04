#ifndef HOOK_H
#define HOOK_H

#include "unit.h"
#include "limit.h"

#define HOOKSIZ     1000

// "HOOKCMD = add struct create",
struct hook {
    char hook[10];
    char cmd[10];
    char cmdopt[10];
    char pgname[10];
    char pgncmd[10];
    char pgnopt[10];
};

struct hooks {
    int size;
    struct hook hook[NHOOKS];
};

int isplugin(const char *pgn);
int plugin(char *env, char *id, char *pgname, char *pgncmd);

int pgm(int argc, char **argv);

// roach: maybe these function names are better? Just sayin'
int ispgn(char *pgn);
int pgnexe(int argc, char **argv);
int hookact(char *command, char *env, char *id);
struct unit *hookcat(struct unit *unit, char *env, char *id);
char *hookls(char *pgn, char *env, char *id);

#endif
