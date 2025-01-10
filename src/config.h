#ifndef CONFIG_H
#define CONFIG_H

#include "pgn.h"
#include "unit.h"
#include "col.h"
#include "common.h"

#define NUMCONFIG       2
#define CONFIGSIZ       256

// FIXME: add an expression max_hookact + max_hookcat + max_hookls
#define CONF_MAXHOOK    10
#define CONF_MAXCOLDEF  10
#define CONF_MAXBASE    256
#define CONF_MAXPGNINS  256

struct config {
    int usehooks;            /* execute hooks from config, by default set */
    char base[100];
    char pgnins[100];
    struct pgns hooks;
    struct columns columns;
};

extern struct config config;

int config_init(void);
int config_deinit(struct config *config);

#endif
