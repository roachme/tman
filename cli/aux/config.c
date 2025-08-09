#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libconfig.h>

#include "../../lib/src/osdep.h"
#include "config.h"
#include "../cli.h"

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

static int tman_config_set_default_base(tman_cfg_t * tman_config)
{
    char pathname[PATH_MAX + 1] = { 0 };

    if (tman_config->base.task == NULL) {
        resolve_env_var_home(pathname, "$HOME/tmantask");
        tman_config->base.task = strdup(pathname);
    }
    if (tman_config->base.pgn == NULL) {
        resolve_env_var_home(pathname, "$HOME/.local/lib/tman/pgn");
        tman_config->base.pgn = strdup(pathname);
    }
    return 0;
}

static int tman_config_set_default_config(tman_cfg_t * tman_config)
{
    tman_config_set_default_base(tman_config);
    return 0;
}

static struct tman_hook *make_hook()
{
    struct tman_hook *hook;

    if ((hook = malloc(sizeof(struct tman_hook))) == NULL)
        return NULL;
    memset(hook, 0, sizeof(struct tman_hook));
    return hook;
}

static int tman_config_get_hooks(config_t * cfg, tman_cfg_t * tman_config)
{
    config_setting_t *setting;

    if ((setting = config_lookup(cfg, "hooks.show")) != NULL) {
        unsigned int count = config_setting_length(setting);
        for (unsigned int i = 0; i < count; ++i) {
            struct tman_hook *hook;
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
                hook->next = tman_config->hooks;
                tman_config->hooks = hook;
            }
        }
    }

    if ((setting = config_lookup(cfg, "hooks.action")) != NULL) {
        unsigned int count = config_setting_length(setting);
        for (unsigned int i = 0; i < count; ++i) {
            struct tman_hook *hook;
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
                hook->next = tman_config->hooks;
                tman_config->hooks = hook;
            }
        }
    }
    return 0;
}

static int tman_config_get_base(config_t * cfg, tman_cfg_t * tman_config)
{
    const char *task, *pgn;
    config_setting_t *setting;

    task = pgn = NULL;
    if ((setting = config_lookup(cfg, "base")) != NULL) {
        char pathname[PATH_MAX + 1];
        if (config_setting_lookup_string(setting, "task", &task)) {
            pathname[0] = '\0'; /* unset pathname value */
            resolve_env_var_home(pathname, task);
            tman_config->base.task = strdup(pathname);
        }
        if (config_setting_lookup_string(setting, "pgn", &pgn)) {
            pathname[0] = '\0'; /* unset pathname value */
            resolve_env_var_home(pathname, pgn);
            tman_config->base.pgn = strdup(pathname);
        }
    }
    return tman_config_set_default_base(tman_config);
}

static int tman_config_get_options(config_t * cfg, tman_cfg_t * tman_config)
{
    config_setting_t *setting;
    tman_config->opts.color = FALSE;
    tman_config->opts.debug = FALSE;
    tman_config->opts.hook = FALSE;

    if ((setting = config_lookup(cfg, "options")) == NULL)
        return 0;

    config_setting_lookup_bool(setting, "hook", &tman_config->opts.hook);
    config_setting_lookup_bool(setting, "color", &tman_config->opts.color);
    config_setting_lookup_bool(setting, "debug", &tman_config->opts.debug);
    return 0;
}

static int parseconf(tman_cfg_t * tman_config, const char *fname)
{
    config_t cfg;

    config_init(&cfg);
    if (!config_read_file(&cfg, fname)) {
        return elog(1, "%s:%d - %s", config_error_file(&cfg),
                    config_error_line(&cfg), config_error_text(&cfg));
    }

    if (tman_config_get_base(&cfg, tman_config))
        elog(1, "tman_config_get_base: FAILED\n");
    else if (tman_config_get_options(&cfg, tman_config))
        elog(1, "tman_config_get_options: FAILED\n");
    else if (tman_config_get_hooks(&cfg, tman_config))
        elog(1, "tman_config_get_hooks: FAILED\n");

    config_destroy(&cfg);
    return 0;
}

int tman_config_init(tman_cfg_t * tman_config)
{
    tman_config->opts.color = NONEBOOL;
    tman_config->opts.debug = NONEBOOL;
    tman_config->opts.hook = NONEBOOL;
    return 0;
}

int tman_config_parse(tman_cfg_t * tman_config)
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
            return parseconf(tman_config, cfgfile);
    }
    return tman_config_set_default_config(tman_config);
}

int tman_config_set_base(tman_base_t * base)
{
    if (base->task != NULL) {
        free(tmancfg.base.task);
        tmancfg.base.task = strdup(base->task);
    }
    if (base->pgn != NULL) {
        free(tmancfg.base.pgn);
        tmancfg.base.pgn = strdup(base->pgn);
    }
    return 0;
}

int tman_config_set_options(tman_opt_t * opts)
{
    tmancfg.opts.hook = opts->hook != NONEBOOL ? opts->hook : tmancfg.opts.hook;
    tmancfg.opts.color =
        opts->color != NONEBOOL ? opts->color : tmancfg.opts.color;
    tmancfg.opts.debug =
        opts->debug != NONEBOOL ? opts->debug : tmancfg.opts.debug;
    return 0;
}

void tman_config_destroy(tman_cfg_t * tman_config)
{
    struct tman_hook *head;

    for (head = tman_config->hooks; head != NULL;) {
        struct tman_hook *tmp = head;
        head = head->next;
        free(tmp);
    }
    free(tman_config->base.task);
    free(tman_config->base.pgn);
}
