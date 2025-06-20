#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "config.h"

static char *get_plugin_description(const char *fname)
{
    FILE *fp;
    char *ptr;
    static char desc[100];

    if ((fp = fopen(fname, "r")) == NULL)
        return strcpy(desc, "some plugin description");

    if (fgets(desc, 100, fp)) {
        if ((ptr = strstr(desc, "\n"))) {
            *ptr = '\0';
        }
    }
    fclose(fp);
    return desc;
}

static int _pgm_chk(int argc, char **argv, struct tman_context *ctx)
{
    printf("check plugins\n");
    return LIBTMAN_OK;
}

static int _pgm_list(int argc, char **argv, struct tman_context *ctx)
{
    DIR *dir;
    struct dirent *pgn;
    const char *pgmfmt = "%s\t%s\t%s\n";
    const char *pgmheader = "Name\tStatus\tDescription";

    if ((dir = opendir(tmancfg->base.pgn)) == NULL)
        return elog(1, "could not open plugin directory");

    printf("%s\n", pgmheader);
    while ((pgn = readdir(dir)) != NULL) {
        char fname[PATH_MAX + 1];

        if (pgn->d_name[0] == '.' || pgn->d_type != DT_DIR)
            continue;
        sprintf(fname, "%s/%s/description", tmancfg->base.pgn, pgn->d_name);
        printf(pgmfmt, pgn->d_name, "inst", get_plugin_description(fname));
    }
    closedir(dir);
    return LIBTMAN_OK;
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
