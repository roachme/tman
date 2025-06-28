#ifndef TMAN_HOOK_H
#define TMAN_HOOK_H

#include "../lib/src/libtman.h"

// TODO: It's duplicate copied from unit.h in lib part. Get rid of it
#define KEYSIZ 20
#define VALSIZ 80

// FIXME: get rid of magic numbers. Might cause buffer overflow
struct tman_hook {
    char cmd[10];
    char cmdopt[10];
    char pgntag[10];
    char pgname[10];
    char pgncmd[10];
    char pgnopt[10];
    struct tman_hook *next;
};

int tman_hook_action(struct tman_arg *args, char *cmd);
int tman_hook_show(struct tman_context *ctx, struct tman_arg *args, char *cmd);
/* TODO: under development.  */
char *tman_hookls(struct tman_hook *hooks, char *pgnout, char *prj, char *id);

#endif
