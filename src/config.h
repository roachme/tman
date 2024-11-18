#ifndef CONFIG_H
#define CONFIG_H

#include "hook.h"
#include "unit.h"

#define NUMCONFIG       2
#define MAXCONFIG       256

// FIXME: add an expression max_hookact + max_hookcat + max_hookls
#define CONF_MAXHOOK    10
#define CONF_MAXBASE    256
#define CONF_MAXPGNINS  256

extern struct config config;

struct config {
    int hooknum;
    char base[100];
    char pgnins[100];
    struct hook hooks[CONF_MAXHOOK];
};

int config_init(void);

#endif
