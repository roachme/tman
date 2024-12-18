#include <stdio.h>
#include <string.h>

#include "pgm.h"
// TODO: remove the header below
#include "cli.h"
#include "../src/tman.h"

builtin_t pgmcmds[] = {
    { .name = "chk",  .func = &_pgm_chk  },
    { .name = "list", .func = &_pgm_list },
};

int pgmcmd_size = sizeof(pgmcmds) / sizeof(pgmcmds[0]);

int _pgm_chk(int argc, char **argv)
{
    printf("check plugins\n");
    return 1;
}

int _pgm_list(int argc, char **argv)
{
    printf("list installed plugins\n");
    return 1;
}

int tman_cli_pgm(int argc, char **argv)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < pgmcmd_size; ++i)
        if (strncmp(cmd, pgmcmds[i].name, CMDSIZ) == 0)
            return pgmcmds[i].func(argc - 1, argv + 1);
    return elog(1, "no such pgm command '%s'", cmd);
}