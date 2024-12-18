#include <string.h>

#include "cli.h"
#include "../src/tman.h"

builtin_t builtins[] = {
    /* system commands */
    { .name = "cfg",  .func = &tman_cfg  },
    { .name = "chk",  .func = &tman_chk  },
    { .name = "init", .func = &tman_init },

    /* basic commands */
    { .name = "add",  .func = &tman_add  },
    { .name = "col",  .func = &tman_col  },
    { .name = "del",  .func = &tman_del  },
    { .name = "prev", .func = &tman_prev },
    { .name = "set",  .func = &tman_set  },
    { .name = "sync", .func = &tman_sync },
    { .name = "use",  .func = &tman_use  },

    /* misc commands */
    { .name = "env",  .func = &tman_env  },
    { .name = "pgm",  .func = &tman_pgm  },
    // maybe make a move command a env subcommand?
    { .name = "move", .func = &tman_move },

    /* info commands */
    { .name = "cat",  .func = &tman_cat  },
    { .name = "help", .func = &tman_help },
    { .name = "list", .func = &tman_list },
    { .name = "ver",  .func = &tman_ver  },
};

int builtin_size = sizeof(builtins) / sizeof(builtins[0]);

int tman_help(int argc, char **argv)
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

int tman_ver(int argc, char **argv)
{
    printf("%s: %s\n", PROGRAM, VERSION);
    return 1;
}

int main(int argc, char **argv)
{
    const char *cmd = argc > 1 ? argv[1] : "list";

    if (tman_init_core(cmd))
        return 1;

    for (int i = 0; i < builtin_size; ++i)
        if (strcmp(cmd, builtins[i].name) == 0)
            return builtins[i].func(argc - 1, argv + 1);

    if (tman_isplugin(cmd))
        return tman_plugin_exec(argc, argv);

    return elog(1, "cannot access '%s': no such command or plugin", cmd);
}
