#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <dirent.h>

#include "env.h"
#include "../core/tman.h"
#include "../core/core.h"
#include "../core/state.h"
#include "../core/common.h"

builtin_t envcmds[] = {
    { .name = "add",  .func = &_env_add  },
    { .name = "del",  .func = &_env_del  },
    { .name = "list", .func = &_env_list },
    { .name = "prev", .func = &_env_prev },
    { .name = "set",  .func = &_env_set  },
    { .name = "use",  .func = &_env_use  },
};

int envcmd_size = sizeof(envcmds) / sizeof(envcmds[0]);


int _env_add(int argc, char **argv)
{
    char c;
    int o_strict = 0;
    int retcode;

    while ((c = getopt(argc, argv, "s")) != -1) {
        switch (c) {
            case 'o':
                o_strict = 1; break ;
        }
    }

    if (optind == argc)
        return elog(1, "%s: task env required\n", PROGRAM);

    for (int i = optind; i < argc; ++i) {
        char *env = argv[i];
        retcode = core_env_add(env);
        if (o_strict && retcode != 0)
            break;
    }
    return retcode == 0 ? core_currdir() : 1;
}

int _env_del(int argc, char **argv)
{
    return 0;
}

int _env_list(int argc, char **argv)
{
    char mark = '+';
    struct dirent *ent;
    DIR *edir = opendir(tmanfs.task);

    if (!edir)
        return elog(1, "could not open task directory\n");

    while ((ent = readdir(edir)) != NULL) {
        if ((strcmp(ent->d_name, ".") == 0) || (strcmp(ent->d_name, "..") == 0))
            continue;

        // TODO: simplify this logic
        if (strcmp(state_getcenv(), ent->d_name) == 0)
            mark = '*';
        else if (strcmp(state_getpenv(), ent->d_name) == 0)
            mark = '^';
        else
            mark = '+';
        printf("[%c] %-10s [%s] %s\n", mark, ent->d_name, "roach", "some env desc");
    }
    return 1;
}

int _env_prev(int argc, char **argv)
{
    if (core_env_prev() != 0)
        return 1;
    return core_currdir();
}

int _env_set(int argc, char **argv)
{
    return 0;
}

int _env_use(int argc, char **argv)
{
    char *env = argc > 1 ? argv[1] : NULL;

    if (core_env_use(env))
        return 1;
    return core_currdir();
}
int tman_env(int argc, char **argv)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < envcmd_size; ++i)
        if (strncmp(cmd, envcmds[i].name, CMDSIZ) == 0)
            return envcmds[i].func(argc - 1, argv + 1);

    return elog(1, "no such env command '%s'", cmd);
}