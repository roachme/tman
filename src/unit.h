#ifndef TMAN_UNIT_H
#define TMAN_UNIT_H

#include "common.h"

#define NKEYS       4
#define KEYSIZ      10
#define VALSIZ      80 /* this big cuz of task description */

struct unitbin {
    int isset;
    char key[KEYSIZ + 1];
    char val[VALSIZ + 1];
};

struct unitpgn {
    struct unitbin node;
    struct unitpgn *next;
};

struct units {
    char id[IDSIZ + 1];
    struct unitpgn *pgn;
    struct unitbin bin[NKEYS];
};

int unit_check(struct unitbin *unitbin);
int unit_addbin(char *env, char *id, struct unitbin *units);
int unit_setbin(char *env, char *id, struct unitbin *units);
struct unitbin *unit_getbin(struct unitbin *units, char *env, char *id);
int unit_delbin(char *env, char *id);

struct unitpgn *unit_addpgn(struct unitpgn *head, char *key, char *val);
int unit_delpgn(struct unitpgn *pgn);

#endif
