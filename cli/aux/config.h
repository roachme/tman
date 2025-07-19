#ifndef CONFIG_H
#define CONFIG_H

#include <libconfig.h>

#include "../../lib/src/common.h"
#include "../../lib/src/libtman.h"

#define NUMCONFIG       2
#define CONFIGSIZ       256

// FIXME: add an expression max_hookact + max_hookshow + max_hookls
#define CONF_MAXHOOK    10
#define CONF_MAXCOLDEF  10
#define CONF_MAXBASE    256
#define CONF_MAXPGNINS  256

struct config {
    int usecolor;               /* use colors */
    int usedebug;               /* output debug info */
    int usehooks;               /* execute hooks from config, by default set */
    tman_base_t base;
    char *pgndir;
    struct tman_hook *hooks;
};

struct config *myconfig_create(void);
extern struct config *tmancfg;
int tmancfg_parse(struct config *config);
void myconfig_destroy(struct config *myconfig);
#endif
