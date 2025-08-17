#ifndef CONFIG_H
#define CONFIG_H

#include <libconfig.h>

#include "../../lib/src/libtec.h"

#define NUMCONFIG       2
#define CONFIGSIZ       256

// FIXME: add an expression max_hookact + max_hookshow + max_hookls
#define CONF_MAXHOOK    10
#define CONF_MAXCOLDEF  10
#define CONF_MAXBASE    256
#define CONF_MAXPGNINS  256

typedef struct tec_base {
    char *pgn;                  /* Directory where plugins are stored */
    char *task;                 /* Directory where tasks are stored */
} tec_base_t;

typedef struct tec_option {
    int color;                  /* use colors */
    int debug;                  /* output debug info */
    int hook;                   /* execute hooks from config, by default set */
} tec_opt_t;

typedef struct config {
    tec_opt_t opts;
    tec_base_t base;
    struct tec_hook *hooks;
} tec_cfg_t;

extern tec_cfg_t teccfg;

int tec_config_init(tec_cfg_t * tec_config);
int tec_config_set_base(tec_base_t * base);
int tec_config_set_options(tec_opt_t * opts);
int tec_config_parse(tec_cfg_t * tec_config);
void tec_config_destroy(tec_cfg_t * tec_config);
#endif
