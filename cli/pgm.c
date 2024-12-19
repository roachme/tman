#include <stdio.h>
#include <string.h>

#include "pgm.h"
#include "cli.h"

static const builtin_t pgmcmds[] = {
    { .name = "chk",  .func = &_pgm_chk  },
    { .name = "list", .func = &_pgm_list },
};

int _pgm_chk(int argc, char **argv, struct tman_context *ctx)
{
    printf("check plugins\n");
    return 1;
}

int _pgm_list(int argc, char **argv, struct tman_context *ctx)
{
    printf("list installed plugins\n");
    return 1;
}

int tman_cli_pgm(int argc, char **argv, struct tman_context *ctx)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < ARRAY_SIZE(pgmcmds); ++i)
        if (strncmp(cmd, pgmcmds[i].name, CMDSIZ) == 0)
            return pgmcmds[i].func(argc - 1, argv + 1, ctx);
    return elog(1, "no such pgm command '%s'", cmd);
}
