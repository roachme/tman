#include <string.h>

#include "cli.h"

static const builtin_t builtins[] = {
    { .name = "add",  .func = &tman_cli_add  },
    { .name = "cat",  .func = &tman_cli_cat  },
    { .name = "cfg",  .func = &tman_cli_cfg  },
    { .name = "chk",  .func = &tman_cli_chk  },
    { .name = "col",  .func = &tman_cli_col  },
    { .name = "del",  .func = &tman_cli_del  },
    { .name = "env",  .func = &tman_cli_env  },
    { .name = "help", .func = &tman_cli_help },
    { .name = "init", .func = &tman_cli_init },
    { .name = "list", .func = &tman_cli_list },
    { .name = "move", .func = &tman_cli_move },
    { .name = "pgm",  .func = &tman_cli_pgm  },
    { .name = "prev", .func = &tman_cli_prev },
    { .name = "set",  .func = &tman_cli_set  },
    { .name = "sync", .func = &tman_cli_sync },
    { .name = "use",  .func = &tman_cli_use  },
    { .name = "ver",  .func = &tman_cli_ver  },
};

int main(int argc, char **argv)
{
    int i, status, cmdfound;
    struct tman_context *ctx;
    const char *cmd = argc > 1 ? argv[1] : "list";

    ctx = NULL;
    cmdfound = FALSE;
    if ((ctx = tman_init(cmd)) == NULL) {
        elog(1, "could not init util: %s", tman_strerror());
        return 1;
    }

    for (i = 0; i < ARRAY_SIZE(builtins); ++i)
        if (strcmp(cmd, builtins[i].name) == 0) {
            cmdfound = TRUE;
            status = builtins[i].func(argc - 1, argv + 1, ctx);
            break;
        }
    if (cmdfound == FALSE && (status = tman_isplugin(cmd)) == TMAN_OK) {
        cmdfound = TRUE;
        status = tman_plugin(argc, argv);
    }
    if (cmdfound == FALSE) {
        status = 1;
        elog(1, "cannot access '%s': no such command or plugin", cmd);
    }

    tman_deinit(ctx);
    return status;
}
