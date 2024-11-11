#ifndef HOOK_H
#define HOOK_H

#include "unit.h"

// "HOOKCMD = add struct create",
struct hookcmd {
    char hook[10];
    char cmd[10];
    char cmdopt[10];
    char pgname[10];
    char pgncmd[10];
    char pgnopt[10];
};


int isplugin(char *pgn);
int plugin(int argc, char **argv);

int pgm(int argc, char **argv);

// roach: maybe these function names are better? Just sayin'
int ispgn(char *pgn);
int pgnexe(int argc, char **argv);
int hookact(char *command, char *env, char *id);
struct punit *hookcat(struct punit *unit, char *env, char *id);
char *hookls(char *pgn, char *env, char *id);

#endif
