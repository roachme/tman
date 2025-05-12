#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "cli.h"

int _pgm_chk(int argc, char **argv, struct tman_context *ctx)
{
    printf("check plugins\n");
    return TMAN_OK;
}

int _pgm_list(int argc, char **argv, struct tman_context *ctx)
{
    DIR *dir;
    struct dirent *pgn;
    const char *pgmfmt = "%s\t%s\t%s\n";
    const char *pgmheader = "Name\tStatus\tDescription";

    if ((dir = opendir(tmanfs.pgnins)) == NULL)
        return elog(1, "could not open plugin directory");

    printf("%s\n", pgmheader);
    while ((pgn = readdir(dir)) != NULL) {
        if (pgn->d_name[0] == '.' || pgn->d_type != DT_DIR)
            continue;
        printf(pgmfmt, pgn->d_name, "inst", "some plugin description");
    }
    closedir(dir);
    return TMAN_OK;
}

static const builtin_t pgmcmds[] = {
    {.name = "chk",.func = &_pgm_chk},
    {.name = "list",.func = &_pgm_list},
};

int tman_cli_pgm(int argc, char **argv, struct tman_context *ctx)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < ARRAY_SIZE(pgmcmds); ++i)
        if (strncmp(cmd, pgmcmds[i].name, CMDSIZ) == 0)
            return pgmcmds[i].func(argc - 1, argv + 1, ctx);
    return elog(1, "no such pgm command '%s'", cmd);
}
