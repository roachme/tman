#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libconfig.h>

#include "../lib/osdep.h"
#include "config.h"
#include "cli.h"

// TODO: gotta define default columns: curr, prev, blog, done
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

static int myconfig_set_default_base(struct config *myconfig)
{
    char pathname[PATH_MAX + 1] = { 0 };

    if (myconfig->base.task == NULL) {
        resolve_env_var_home(pathname, "$HOME/tmantask");
        myconfig->base.task = strdup(pathname);
    }
    if (myconfig->base.pgn == NULL) {
        resolve_env_var_home(pathname, "$HOME/.local/lib/tman/pgn");
        myconfig->base.pgn = strdup(pathname);
    }
    return 0;
}

static int myconfig_set_default_config(struct config *myconfig)
{
    myconfig_set_default_base(myconfig);
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

static int myconfig_get_hooks(config_t * cfg, struct config *myconfig)
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
                hook->next = myconfig->hooks;
                myconfig->hooks = hook;
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
                hook->next = myconfig->hooks;
                myconfig->hooks = hook;
            }
        }
    }
    return 0;
}

static int myconfig_get_base(config_t * cfg, struct config *myconfig)
{
    const char *task, *pgn;
    char pathname[PATH_MAX + 1];
    config_setting_t *setting;

    task = pgn = NULL;
    if ((setting = config_lookup(cfg, "base")) != NULL) {
        if (config_setting_lookup_string(setting, "task", &task)) {
            pathname[0] = '\0'; /* unset pathname value */
            resolve_env_var_home(pathname, task);
            myconfig->base.task = strdup(pathname);
        }
        if (config_setting_lookup_string(setting, "pgn", &pgn)) {
            pathname[0] = '\0'; /* unset pathname value */
            resolve_env_var_home(pathname, pgn);
            myconfig->base.pgn = strdup(pathname);
        }
    }
    return myconfig_set_default_base(myconfig);
}

static int myconfig_get_options(config_t * cfg, struct config *myconfig)
{
    config_setting_t *setting;
    myconfig->usecolors = FALSE;
    myconfig->usedebug = FALSE;
    myconfig->usehooks = FALSE;

    if ((setting = config_lookup(cfg, "options")) == NULL)
        return 0;

    config_setting_lookup_bool(setting, "hook", &myconfig->usehooks);
    config_setting_lookup_bool(setting, "color", &myconfig->usecolors);
    config_setting_lookup_bool(setting, "debug", &myconfig->usedebug);
    return 0;
}

static int parseconf(struct config *myconfig, const char *fname)
{
    config_t cfg;

    config_init(&cfg);
    if (!config_read_file(&cfg, fname)) {
        return elog(1, "%s:%d - %s", config_error_file(&cfg),
                    config_error_line(&cfg), config_error_text(&cfg));
    }

    if (myconfig_get_base(&cfg, myconfig))
        elog(1, "myconfig_get_base: FAILED\n");
    else if (myconfig_get_options(&cfg, myconfig))
        elog(1, "myconfig_get_options: FAILED\n");
    else if (myconfig_get_hooks(&cfg, myconfig))
        elog(1, "myconfig_get_hooks: FAILED\n");

    config_destroy(&cfg);
    return 0;
}

struct config *myconfig_create(void)
{
    struct config *config;

    if ((config = malloc(sizeof(struct config))) == NULL)
        return 0;
    memset(config, 0, sizeof(struct config));
    return config;
}

int tmancfg_parse(struct config *myconfig)
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
            return parseconf(myconfig, cfgfile);
    }
    return myconfig_set_default_config(myconfig);
}

void myconfig_destroy(struct config *myconfig)
{
    struct tman_hook *head;

    for (head = myconfig->hooks; head != NULL;) {
        struct tman_hook *tmp = head;
        head = head->next;
        free(tmp);
    }
    free(myconfig->base.task);
    free(myconfig->base.pgn);
    free(myconfig);
}
