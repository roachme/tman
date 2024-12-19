#include <string.h>

#include "cli.h"
#include "../src/tman.h"

builtin_t builtins[] = {
    /* system commands */
    { .name = "cfg",  .func = &tman_cli_cfg  },
    { .name = "chk",  .func = &tman_cli_chk  },
    { .name = "init", .func = &tman_cli_init },

    /* basic commands */
    { .name = "add",  .func = &tman_cli_add  },
    { .name = "col",  .func = &tman_cli_col  },
    { .name = "del",  .func = &tman_cli_del  },
    { .name = "prev", .func = &tman_cli_prev },
    { .name = "set",  .func = &tman_cli_set  },
    { .name = "sync", .func = &tman_cli_sync },
    { .name = "use",  .func = &tman_cli_use  },

    /* misc commands */
    { .name = "env",  .func = &tman_cli_env  },
    { .name = "pgm",  .func = &tman_cli_pgm  },
    // maybe make a move command a env subcommand?
    { .name = "move", .func = &tman_cli_move },

    /* info commands */
    { .name = "cat",  .func = &tman_cli_cat  },
    { .name = "help", .func = &tman_cli_help },
    { .name = "list", .func = &tman_cli_list },
    { .name = "ver",  .func = &tman_cli_ver  },
};

int builtin_size = sizeof(builtins) / sizeof(builtins[0]);

int tman_cli_help(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    char *cmd;
    char *key = NULL;

    while ((c = getopt(argc, argv, "dk:")) != -1) {
        switch (c) {
            case 'k':
                key = optarg; break;
        };
    }

    cmd = argv[optind];
    return help_lookup(cmd);
}

int tman_cli_ver(int argc, char **argv, struct tman_context *ctx)
{
    printf("%s: %s\n", PROGRAM, VERSION);
    return TMAN_OK;
}

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

    for (i = 0; i < builtin_size; ++i)
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
