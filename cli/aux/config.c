#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libconfig.h>

#include "../../lib/src/osdep.h"
#include "config.h"
#include "../tec.h"

// TODO: gotta define default columns: curr, prev, blog
// TODO: gotta add config checker so a program doesn't fail.

static void resolve_env_var_home(char *dst, const char *src)
{
    char *home = getenv("HOME");
    char *found = strstr(src, "$HOME");

    if (found) {
        strcpy(dst, home);      /* TODO: it might be unset in some envs.  */
        strcat(dst, src + strlen("$HOME"));
    } else {
        strcpy(dst, src);
    }
}

static int tec_config_set_default_base(tec_cfg_t * tec_config)
{
    char pathname[PATH_MAX + 1] = { 0 };

    if (tec_config->base.task == NULL) {
        resolve_env_var_home(pathname, "$HOME/tectask");
        tec_config->base.task = strdup(pathname);
    }
    if (tec_config->base.pgn == NULL) {
        resolve_env_var_home(pathname, "$HOME/.local/lib/tec/pgn");
        tec_config->base.pgn = strdup(pathname);
    }
    return 0;
}

static int tec_config_set_default_config(tec_cfg_t * tec_config)
{
    tec_config_set_default_base(tec_config);
    return 0;
}

static struct tec_hook *make_hook()
{
    struct tec_hook *hook;

    if ((hook = malloc(sizeof(struct tec_hook))) == NULL)
        return NULL;
    memset(hook, 0, sizeof(struct tec_hook));
    return hook;
}

static int tec_config_get_hooks(config_t * cfg, tec_cfg_t * tec_config)
{
    config_setting_t *setting;

    if ((setting = config_lookup(cfg, "hooks.show")) != NULL) {
        unsigned int count = config_setting_length(setting);
        for (unsigned int i = 0; i < count; ++i) {
            struct tec_hook *hook;
            const char *bincmd, *pgname, *pgncmd;
            config_setting_t *hook_conf = config_setting_get_elem(setting, i);

            if ((hook = make_hook()) != NULL) {
                if (!(config_setting_lookup_string(hook_conf, "bincmd", &bincmd)
                      && config_setting_lookup_string(hook_conf, "pgname",
                                                      &pgname)
                      && config_setting_lookup_string(hook_conf, "pgncmd",
                                                      &pgncmd))) {
                    elog(1, "FAILED: to parse hook.shows");
                    continue;
                }
                strcpy(hook->pgntag, "HOOKSHOW");
                strcpy(hook->cmd, bincmd);
                strcpy(hook->pgname, pgname);
                strcpy(hook->pgncmd, pgncmd);
                hook->next = tec_config->hooks;
                tec_config->hooks = hook;
            }
        }
    }

    if ((setting = config_lookup(cfg, "hooks.action")) != NULL) {
        unsigned int count = config_setting_length(setting);
        for (unsigned int i = 0; i < count; ++i) {
            struct tec_hook *hook;
            const char *bincmd, *pgname, *pgncmd;
            config_setting_t *hook_conf = config_setting_get_elem(setting, i);

            if ((hook = make_hook()) != NULL) {
                if (!(config_setting_lookup_string(hook_conf, "bincmd", &bincmd)
                      && config_setting_lookup_string(hook_conf, "pgname",
                                                      &pgname)
                      && config_setting_lookup_string(hook_conf, "pgncmd",
                                                      &pgncmd))) {
                    elog(1, "FAILED: to parse hook.shows");
                    continue;
                }
                strcpy(hook->pgntag, "HOOKCMD");
                strcpy(hook->cmd, bincmd);
                strcpy(hook->pgname, pgname);
                strcpy(hook->pgncmd, pgncmd);
                hook->next = tec_config->hooks;
                tec_config->hooks = hook;
            }
        }
    }
    return 0;
}

static int tec_config_get_base(config_t * cfg, tec_cfg_t * tec_config)
{
    const char *task, *pgn;
    config_setting_t *setting;

    task = pgn = NULL;
    if ((setting = config_lookup(cfg, "base")) != NULL) {
        char pathname[PATH_MAX + 1];
        if (config_setting_lookup_string(setting, "task", &task)) {
            pathname[0] = '\0'; /* unset pathname value */
            resolve_env_var_home(pathname, task);
            tec_config->base.task = strdup(pathname);
        }
        if (config_setting_lookup_string(setting, "pgn", &pgn)) {
            pathname[0] = '\0'; /* unset pathname value */
            resolve_env_var_home(pathname, pgn);
            tec_config->base.pgn = strdup(pathname);
        }
    }
    return tec_config_set_default_base(tec_config);
}

static int tec_config_get_options(config_t * cfg, tec_cfg_t * tec_config)
{
    config_setting_t *setting;
    tec_config->opts.color = false;
    tec_config->opts.debug = false;
    tec_config->opts.hook = false;

    if ((setting = config_lookup(cfg, "options")) == NULL)
        return 0;

    config_setting_lookup_bool(setting, "hook", &tec_config->opts.hook);
    config_setting_lookup_bool(setting, "color", &tec_config->opts.color);
    config_setting_lookup_bool(setting, "debug", &tec_config->opts.debug);
    return 0;
}

static int parseconf(tec_cfg_t * tec_config, const char *fname)
{
    config_t cfg;

    config_init(&cfg);
    if (!config_read_file(&cfg, fname)) {
        return elog(1, "%s:%d - %s", config_error_file(&cfg),
                    config_error_line(&cfg), config_error_text(&cfg));
    }

    if (tec_config_get_base(&cfg, tec_config))
        elog(1, "tec_config_get_base: FAILED\n");
    else if (tec_config_get_options(&cfg, tec_config))
        elog(1, "tec_config_get_options: FAILED\n");
    else if (tec_config_get_hooks(&cfg, tec_config))
        elog(1, "tec_config_get_hooks: FAILED\n");

    config_destroy(&cfg);
    return 0;
}

int tec_config_init(tec_cfg_t * tec_config)
{
    tec_config->opts.color = NONEBOOL;
    tec_config->opts.debug = NONEBOOL;
    tec_config->opts.hook = NONEBOOL;
    return 0;
}

int tec_config_parse(tec_cfg_t * tec_config)
{
    char cfgfile[CONFIGSIZ + 1];
    char *homedir = getenv("HOME");
    const char cfgfmts[NUMCONFIG][CONFIGSIZ + 1] = {
        "%s/.%s/%s.cfg",
        "%s/.config/%s/%s.cfg",
    };

    for (int i = 0; i < NUMCONFIG; ++i) {
        sprintf(cfgfile, cfgfmts[i], homedir, PROGRAM, PROGRAM);
        if (ISFILE(cfgfile))
            return parseconf(tec_config, cfgfile);
    }
    return tec_config_set_default_config(tec_config);
}

int tec_config_set_base(tec_base_t * base)
{
    if (base->task != NULL) {
        free(teccfg.base.task);
        teccfg.base.task = strdup(base->task);
    }
    if (base->pgn != NULL) {
        free(teccfg.base.pgn);
        teccfg.base.pgn = strdup(base->pgn);
    }
    return 0;
}

int tec_config_set_options(tec_opt_t * opts)
{
    teccfg.opts.hook = opts->hook != NONEBOOL ? opts->hook : teccfg.opts.hook;
    teccfg.opts.color =
        opts->color != NONEBOOL ? opts->color : teccfg.opts.color;
    teccfg.opts.debug =
        opts->debug != NONEBOOL ? opts->debug : teccfg.opts.debug;
    return 0;
}

void tec_config_destroy(tec_cfg_t * tec_config)
{
    struct tec_hook *head;

    for (head = tec_config->hooks; head != NULL;) {
        struct tec_hook *tmp = head;
        head = head->next;
        free(tmp);
    }
    free(tec_config->base.task);
    free(tec_config->base.pgn);
}
