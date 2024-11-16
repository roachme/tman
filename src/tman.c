#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/stat.h>

#include "tman.h"
#include "core.h"
#include "hook.h"
#include "help.h"
#include "config.h"
#include "common.h"

struct tmanstruct tmanfs;

builtin_t builtins[] = {
    /* system commands */
    { .name = "cfg",  .func = &tman_cfg  },
    { .name = "init", .func = &tman_init },

    /* basic commands */
    { .name = "add",  .func = &tman_add  },
    { .name = "del",  .func = &tman_del  },
    { .name = "prev", .func = &tman_prev },
    { .name = "set",  .func = &tman_set  },
    { .name = "sync", .func = &tman_sync },
    { .name = "use",  .func = &tman_use  },

    /* misc commands */
    { .name = "env",  .func = &tman_env  },
    { .name = "pgm",  .func = &tman_pgm  },
    { .name = "move", .func = &tman_move },

    /* info commands */
    { .name = "cat",  .func = &tman_cat  },
    { .name = "help", .func = &tman_help },
    { .name = "list", .func = &tman_list },
    { .name = "ver",  .func = &tman_ver  },
};

int builtin_size = sizeof(builtins) / sizeof(builtins[0]);

int tman_initfs()
{
    sprintf(tmanfs.base,   "%s",    config.base);
    sprintf(tmanfs.db,     "%s/%s", tmanfs.base, ".tman");
    sprintf(tmanfs.finit,  "%s/%s", tmanfs.db,   "inited");
    sprintf(tmanfs.fstate, "%s/%s", tmanfs.db,   "state");
    sprintf(tmanfs.task,   "%s/%s", tmanfs.base, "tasks");
    sprintf(tmanfs.pgn,    "%s/%s", tmanfs.base, "pgns");
    sprintf(tmanfs.pgnins, "%s",    config.pgnins);
    return 0;
}

int tman_help(int argc, char **argv)
{
    char c;
    char *cmd;
    char *key = NULL;
    char o_desc = false;

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

    if (config_init())
        return elog(1, "failed to parse system config file\n");

    /* init util for a command. Not all of 'em need initialization */
    if (tman_initfs() != 1 && core_init(cmd))
        return 1;

    for (int i = 0; i < builtin_size; ++i)
        if (strcmp(cmd, builtins[i].name) == 0)
            return builtins[i].func(argc - 1, argv + 1);

    if (isplugin(cmd))
        return plugin(argc, argv);

    return elog(1, "cannot access '%s': no such command or plugin", cmd);
}
