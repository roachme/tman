#ifndef TMAN_PGN_H
#define TMAN_PGN_H

#include "unit.h"
#include "limit.h"

#define HOOKSIZ     1000

// "HOOKCMD = add struct create",
struct pgn {
    char cmd[10];
    char cmdopt[10];
    char pgntag[10];
    char pgname[10];
    char pgncmd[10];
    char pgnopt[10];
};

struct pgns {
    int size;
    struct pgn pgn[NHOOKS];
};

int ispgn(const char *pgn);
int pgnact(char *command, char *env, char *id);
struct unit *pgncat(struct unit *unit, char *env, char *id);
char *pgnls(char *pgn, char *env, char *id);

#endif
