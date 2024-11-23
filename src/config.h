#ifndef CONFIG_H
#define CONFIG_H

#include "hook.h"
#include "unit.h"
#include "column.h"

#define NUMCONFIG       2
#define MAXCONFIG       256

// FIXME: add an expression max_hookact + max_hookcat + max_hookls
#define CONF_MAXHOOK    10
#define CONF_MAXCOLDEF  10
#define CONF_MAXBASE    256
#define CONF_MAXPGNINS  256

struct hooks {
    int size;
    struct hook hook[CONF_MAXHOOK];
};

struct columns {
    int size;
    struct column column[NCOLUMNS];
};

struct config {
    char base[100];
    char pgnins[100];
    struct hooks hooks;
    struct columns columns;
};

extern struct config config;

int config_init(void);

#endif
