#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "cli.h"
#include "config.h"
#include "help.h"

#define NONEBOOL        -1      /* Not yet set boolean value */

/*
 * Tman util options:
    -c      Use colors
    -h      Use hooks
    -i      Base dir (where DB is stored)
    -p      Plugin dir (where plugins stored)
*/

static int show_version()
{
    printf("%s: %s\n", PROGRAM, VERSION);
    return 0;
}

int elog(int status, const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    fprintf(stderr, PROGRAM ": ");
    vfprintf(stderr, fmt, arg);
    fprintf(stderr, "\n");
    va_end(arg);
    return status;
}

static const builtin_t builtins[] = {
    {.name = "add",.func = &tman_cli_add,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "cfg",.func = &tman_cli_cfg,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "chk",.func = &tman_cli_chk,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "col",.func = &tman_cli_col,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "del",.func = &tman_cli_del,.setuplvl = LIBTMAN_SETUPCHECK},
    //{ .name = "flow", .func = &tman_cli_flow, .setuplvl = LIBTMAN_SETUPCHECK },
    {.name = "help",.func = &tman_cli_help,.setuplvl = LIBTMAN_SETUPSOFT},
    {.name = "init",.func = &tman_cli_init,.setuplvl = LIBTMAN_SETUPHARD},
    {.name = "list",.func = &tman_cli_list,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "move",.func = &tman_cli_move,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "pgm",.func = &tman_cli_pgm,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "prev",.func = &tman_cli_prev,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "prj",.func = &tman_cli_prj,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "set",.func = &tman_cli_set,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "show",.func = &tman_cli_show,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "sync",.func = &tman_cli_sync,.setuplvl = LIBTMAN_SETUPCHECK},
};

struct config *tman_config;

int main(int argc, char **argv)
{
    int usecolors, usedebug, usehooks;
    char *cmd = NULL, *option, *togfmt;
    struct tman_base base = {.task = NULL,.pgn = NULL };
    int i, status, cmdfound;
    struct tman_context *ctx;

    cmd = option = NULL;
    usecolors = usedebug = usehooks = NONEBOOL;
    togfmt = "option `%s' accepts either 'on' or 'off'";

    /* Parse util itself options.  */
    for (i = 1; i < argc && argv[i][0] == '-'; ++i) {
        if ((option = argv[i]) && strcmp(option, "-T") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            base.task = argv[++i];
        } else if ((option = argv[i]) && strcmp(option, "-C") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            if (strcmp(argv[i + 1], "on") == 0)
                usecolors = TRUE;
            else if (strcmp(argv[i + 1], "off") == 0)
                usecolors = FALSE;
            else
                return elog(1, togfmt, option);
            ++i;                /* Skip option.  */
        } else if ((option = argv[i]) && strcmp(option, "-D") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            if (strcmp(argv[i + 1], "on") == 0)
                usedebug = TRUE;
            else if (strcmp(argv[i + 1], "off") == 0)
                usedebug = FALSE;
            else
                return elog(1, togfmt, option);
            ++i;                /* Skip option.  */
            return elog(1, "this option is under development");
        } else if (strcmp(argv[i], "-h") == 0)
            return help_lookup(NULL);
        else if (strcmp(argv[i], "-P") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", argv[i]);
            base.pgn = argv[++i];
        } else if ((option = argv[i]) && strcmp(argv[i], "-H") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", argv[i]);
            if (strcmp(argv[i + 1], "on") == 0)
                usehooks = TRUE;
            else if (strcmp(argv[i + 1], "off") == 0)
                usehooks = FALSE;
            else
                return elog(1, togfmt, option);
            ++i;                /* Skip option.  */
        } else if (strcmp(argv[i], "-V") == 0)
            return show_version();
        else
            return elog(1, "invalid option `%s'", argv[i]);
    }

    if ((cmd = argv[i]) == NULL) {
        help_lookup(NULL);
        return 1;
    }

    if ((tman_config = myconfig_create()) == NULL) {
        fprintf(stderr, "myconfig_create: FAIL\n");
    }

    if (tman_config_parse(tman_config)) {
        fprintf(stderr, "could parse config file\n");
        return 1;
    }

    ctx = NULL;
    cmdfound = FALSE;

    if (base.task != NULL) {
        free(tman_config->base.task);
        tman_config->base.task = strdup(base.task);
    }
    if (base.pgn != NULL) {
        free(tman_config->base.pgn);
        tman_config->base.pgn = strdup(base.pgn);
    }

    tman_config->usehooks =
        usehooks != NONEBOOL ? usehooks : tman_config->usehooks;
    tman_config->usecolors =
        usecolors != NONEBOOL ? usecolors : tman_config->usecolors;
    tman_config->usedebug =
        usedebug != NONEBOOL ? usedebug : tman_config->usedebug;

    if ((ctx = tman_init(&tman_config->base)) == NULL) {
        elog(1, "could not init util: %s", tman_strerror());
        return 1;
    }

    for (int idx = 0; idx < ARRAY_SIZE(builtins); ++idx)
        if (strcmp(cmd, builtins[idx].name) == 0) {
            cmdfound = TRUE;
            if ((status = tman_setup(builtins[idx].setuplvl)) != LIBTMAN_OK) {
                elog(status, "%s", tman_strerror());
                goto out;
            }
            status = builtins[idx].func(argc - i, argv + i, ctx);
            goto out;
        }
    if (cmdfound == FALSE
        && (status = tman_ispgn(tman_config->base.pgn, cmd)) == TRUE) {
        cmdfound = TRUE;
        if ((status = tman_setup(LIBTMAN_SETUPCHECK)) != LIBTMAN_OK) {
            elog(status, "%s", tman_strerror());
            goto out;
        }
        status = tman_cli_plugin(argc, argv, ctx);
    }
    if (cmdfound == FALSE) {
        status = 1;
        elog(1, "%s: no such command or plugin", cmd);
    }

 out:
    ctx = tman_deinit(ctx);
    myconfig_destroy(tman_config);
    return status;
}
