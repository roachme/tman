#ifndef CONFIG_H
#define CONFIG_H

#include <libconfig.h>

#include "../../lib/src/libtman.h"

#define NUMCONFIG       2
#define CONFIGSIZ       256

// FIXME: add an expression max_hookact + max_hookshow + max_hookls
#define CONF_MAXHOOK    10
#define CONF_MAXCOLDEF  10
#define CONF_MAXBASE    256
#define CONF_MAXPGNINS  256

typedef struct tman_base {
    char *pgn;                  /* Directory where plugins are stored */
    char *task;                 /* Directory where tasks are stored */
} tman_base_t;

typedef struct tman_option {
    int color;                  /* use colors */
    int debug;                  /* output debug info */
    int hook;                   /* execute hooks from config, by default set */
} tman_opt_t;

typedef struct config {
    tman_opt_t opts;
    tman_base_t base;
    struct tman_hook *hooks;
} tman_cfg_t;

extern tman_cfg_t tmancfg;

int tman_config_init(tman_cfg_t * tman_config);
int tman_config_set_base(tman_base_t * base);
int tman_config_set_options(tman_opt_t * opts);
int tman_config_parse(tman_cfg_t * tman_config);
void tman_config_destroy(tman_cfg_t * tman_config);
#endif
