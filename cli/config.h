#ifndef CONFIG_H
#define CONFIG_H

#include "col.h"
#include "../lib/common.h"
#include "../lib/tman.h"

#define NUMCONFIG       2
#define CONFIGSIZ       256

// FIXME: add an expression max_hookact + max_hookcat + max_hookls
#define CONF_MAXHOOK    10
#define CONF_MAXCOLDEF  10
#define CONF_MAXBASE    256
#define CONF_MAXPGNINS  256

struct config {
    int usehooks;               /* execute hooks from config, by default set */
    char base[100];
    char pgnins[100];
    struct tman_hook *hooks;
};

extern struct config *tman_config;

struct config *config_init(void);
void *config_deinit(struct config *config);

#endif
