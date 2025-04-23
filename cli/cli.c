#include <string.h>
#include <stdarg.h>

#include "cli.h"
#include "config.h"
#include "help.h"

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
    {.name = "add",.func = &tman_cli_add,.setuplvl = TMAN_SETUPCHECK},
    {.name = "cfg",.func = &tman_cli_cfg,.setuplvl = TMAN_SETUPCHECK},
    {.name = "chk",.func = &tman_cli_chk,.setuplvl = TMAN_SETUPCHECK},
    {.name = "col",.func = &tman_cli_col,.setuplvl = TMAN_SETUPCHECK},
    {.name = "del",.func = &tman_cli_del,.setuplvl = TMAN_SETUPCHECK},
    //{.name = "find",.func = &tman_cli_find,.setuplvl = TMAN_SETUPCHECK},
    //{ .name = "flow", .func = &tman_cli_flow, .setuplvl = TMAN_SETUPCHECK },
    {.name = "help",.func = &tman_cli_help,.setuplvl = TMAN_SETUPSOFT},
    {.name = "init",.func = &tman_cli_init,.setuplvl = TMAN_SETUPHARD},
    //{ .name = "link", .func = &tman_cli_link, .setuplvl = TMAN_SETUPHARD },
    {.name = "list",.func = &tman_cli_list,.setuplvl = TMAN_SETUPCHECK},
    {.name = "move",.func = &tman_cli_move,.setuplvl = TMAN_SETUPCHECK},
    {.name = "pgm",.func = &tman_cli_pgm,.setuplvl = TMAN_SETUPCHECK},
    {.name = "prev",.func = &tman_cli_prev,.setuplvl = TMAN_SETUPCHECK},
    {.name = "prj",.func = &tman_cli_prj,.setuplvl = TMAN_SETUPCHECK},
    {.name = "set",.func = &tman_cli_set,.setuplvl = TMAN_SETUPCHECK},
    {.name = "show",.func = &tman_cli_show,.setuplvl = TMAN_SETUPCHECK},
    {.name = "sync",.func = &tman_cli_sync,.setuplvl = TMAN_SETUPCHECK},
};

struct config *tman_config;

int main(int argc, char **argv)
{
    char *cmd = argv[1];
    struct tman_base base;
    int i, status, cmdfound;
    struct tman_context *ctx;

    if (cmd == NULL) {
        help_lookup(NULL);
        return 1;
    }

    /* Parse util itself options.  */
    for (int i = 1; i < argc && argv[i][0] == '-'; ++i) {
        if (strcmp(argv[i], "-V") == 0)
            return show_version();
        else if (strcmp(argv[i], "-h") == 0)
            return help_lookup(NULL);
        else
            return elog(1, "invalid option `%s'", argv[i]);
    }

    if ((tman_config = config_init()) == NULL) {
        fprintf(stderr, "could malloc memory for config");
        return 1;
    } else if (config_parse(tman_config)) {
        config_deinit(tman_config);
        fprintf(stderr, "could parse config file");
        return 1;
    }

    ctx = NULL;
    cmdfound = FALSE;
    base.base = tman_config->base;
    base.pgn = tman_config->pgnins;

    if ((ctx = tman_init(&base)) == NULL) {
        elog(1, "could not init util: %s", tman_strerror());
        return 1;
    }

    for (i = 0; i < ARRAY_SIZE(builtins); ++i)
        if (strcmp(cmd, builtins[i].name) == 0) {
            cmdfound = TRUE;
            if ((status = tman_setup(builtins[i].setuplvl)) != TMAN_OK) {
                elog(status, "%s", tman_strerror());
                goto out;
            }
            status = builtins[i].func(argc - 1, argv + 1, ctx);
            goto out;
        }
    if (cmdfound == FALSE && (status = tman_ispgn(base.pgn, cmd)) == TRUE) {
        cmdfound = TRUE;
        if ((status = tman_setup(TMAN_SETUPCHECK)) != TMAN_OK) {
            elog(status, "%s", tman_strerror());
            goto out;
        }
        status = tman_cli_plugin(cmd, argc - 1, argv + 1, ctx);
    }
    if (cmdfound == FALSE) {
        status = 1;
        elog(1, "%s: no such command or plugin", cmd);
    }

 out:
    ctx = tman_deinit(ctx);
    tman_config = config_deinit(tman_config);
    return status;
}
