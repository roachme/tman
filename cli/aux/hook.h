#ifndef TMAN_HOOK_H
#define TMAN_HOOK_H

#include "../../lib/src/libtman.h"

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

int hook_action(tman_arg_t * args, char *cmd);
int hook_show(tman_unit_t ** units, tman_arg_t * args, char *cmd);
/* TODO: under development.  */
char *hook_list(struct tman_hook *hooks, char *pgnout, char *prj, char *id);

#endif
