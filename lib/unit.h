#ifndef TMAN_UNIT_H
#define TMAN_UNIT_H

#include "common.h"

#define NKEYS       4
#define KEYSIZ      10
#define VALSIZ      80 /* this big cuz of task description */

struct unit {
    int isset;
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
    struct unit *next;
};

struct units {
    char id[IDSIZ + 1];
    struct unit *pgn;
    struct unit bin[NKEYS];
};

int unit_chkbin(struct unit *units);
int unit_addbin(char *prj, char *id, struct unit *units);
int unit_setbin(char *prj, char *id, struct unit *units);
struct unit *unit_getbin(struct unit *units, char *prj, char *id);
int unit_delbin(char *prj, char *id);

struct unit *unit_addpgn(struct unit *head, char *key, char *val);
int unit_delpgn(struct unit *pgn);

#endif
