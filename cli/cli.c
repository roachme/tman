#include <string.h>

#include "cli.h"

static const builtin_t builtins[] = {
    { .name = "add",  .func = &tman_cli_add,  .setuplvl = TMAN_SETUPCHECK },
    { .name = "cat",  .func = &tman_cli_cat,  .setuplvl = TMAN_SETUPCHECK },
    { .name = "cfg",  .func = &tman_cli_cfg,  .setuplvl = TMAN_SETUPCHECK },
    { .name = "chk",  .func = &tman_cli_chk,  .setuplvl = TMAN_SETUPCHECK },
    { .name = "col",  .func = &tman_cli_col,  .setuplvl = TMAN_SETUPCHECK },
    { .name = "del",  .func = &tman_cli_del,  .setuplvl = TMAN_SETUPCHECK },
    { .name = "env",  .func = &tman_cli_env,  .setuplvl = TMAN_SETUPCHECK },
    { .name = "help", .func = &tman_cli_help, .setuplvl = TMAN_SETUPSOFT },
    { .name = "init", .func = &tman_cli_init, .setuplvl = TMAN_SETUPHARD },
    { .name = "list", .func = &tman_cli_list, .setuplvl = TMAN_SETUPCHECK },
    { .name = "move", .func = &tman_cli_move, .setuplvl = TMAN_SETUPCHECK },
    { .name = "pgm",  .func = &tman_cli_pgm,  .setuplvl = TMAN_SETUPCHECK },
    { .name = "prev", .func = &tman_cli_prev, .setuplvl = TMAN_SETUPCHECK },
    { .name = "set",  .func = &tman_cli_set,  .setuplvl = TMAN_SETUPCHECK },
    { .name = "sync", .func = &tman_cli_sync, .setuplvl = TMAN_SETUPCHECK },
    { .name = "use",  .func = &tman_cli_use,  .setuplvl = TMAN_SETUPCHECK },
    { .name = "ver",  .func = &tman_cli_ver,  .setuplvl = TMAN_SETUPSOFT },
};

int main(int argc, char **argv)
{
    tman_ctx_t *ctx;
    int i, status, cmdfound;
    const char *cmd = argc > 1 ? argv[1] : "list";
    struct tman_pgn_opt pgnopt;

    ctx = NULL;
    cmdfound = FALSE;
    if ((ctx = tman_init()) == NULL) {
        elog(1, "could not init util: %s", tman_strerror());
        return 1;
    }

    for (i = 0; i < ARRAY_SIZE(builtins); ++i)
        if (strcmp(cmd, builtins[i].name) == 0) {
            cmdfound = TRUE;
            if ((status = tman_setup(builtins[i].setuplvl)) != TMAN_OK) {
                elog(status, "setup failed: %s", tman_strerror());
                goto out;
            }
            status = builtins[i].func(argc - 1, argv + 1, ctx);
            goto out;
        }
    if (cmdfound == FALSE && (status = tman_isplugin(cmd)) == TMAN_OK) {
        cmdfound = TRUE;
        status = tman_plugin(ctx, argc, argv, &pgnopt);
    }
    if (cmdfound == FALSE) {
        status = 1;
        elog(1, "cannot access '%s': no such command or plugin", cmd);
    }

out:
    tman_deinit(ctx);
    return status;
}
