#ifndef HOOK_H
#define HOOK_H

#include "unit.h"


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
