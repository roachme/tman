#ifndef TMAN_PGN_H
#define TMAN_PGN_H

#include "unit.h"
#include "limit.h"

#define HOOKSIZ     1000

// "HOOKCMD = add struct create",
struct hook {
    char cmd[10];
    char cmdopt[10];
    char pgntag[10];
    char pgname[10];
    char pgncmd[10];
    char pgnopt[10];
};

struct hooks {
    int size;
    struct hook hook[NHOOKS];
};

int ispgn(const char *pgn);
int hookact(char *command, char *prj, char *id);
struct unit *hookshow(char *prj, char *id);
char *hookls(char *pgnout, char *prj, char *id);

#endif
