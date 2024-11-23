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

struct coldef {
    int prio;
    char mark;
    char env[ENVSIZ];
    char tag[TAGSIZ];
};

struct coldefs {
    int size;
    struct coldef coldef[NCOLUMNS];
};

struct hooks {
    int size;
    struct hook hook[CONF_MAXHOOK];
};

extern struct config config;

struct config {
    char base[100];
    char pgnins[100];
    struct hooks hooks;
    struct coldefs coldefs;
};

int config_init(void);

#endif
