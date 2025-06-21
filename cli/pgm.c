#include <git2.h>
#include <dirent.h>
#include <sys/stat.h>
#include <git2/clone.h>
#include <git2/global.h>
#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "config.h"

#define GITHUB_URL      "https://github.com"

static int is_dir(char *fname)
{
    struct stat st;

    if (stat(fname, &st) == 0 && S_ISDIR(st.st_mode))
        return 1;
    return 0;
}

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

static int _pgm_inst(int argc, char **argv, struct tman_context *ctx)
{
    int status;
    char *name, *tmp;
    char fullurl[200] = { 0 };
    char fulldir[200] = { 0 };
    struct pgncfg *pgncfg;
    git_repository *repo = NULL;
    git_clone_options opts = GIT_CLONE_OPTIONS_INIT;

    for (pgncfg = tmancfg->pgncfg; pgncfg; pgncfg = pgncfg->next) {
        sprintf(fulldir, "%s/%s", GITHUB_URL, pgncfg->name);

        if ((name = strstr(pgncfg->name, "/"))) {
            if ((tmp = strstr(pgncfg->name, "-")))
                name = tmp;
            ++name;             /* skip dash before the name.  */

            sprintf(fullurl, "%s/%s", GITHUB_URL, pgncfg->name);
            sprintf(fulldir, "%s/%s", tmancfg->base.pgn, name);
            printf("fullurl: %s - %s\n", fullurl, name);
            if (is_dir(fulldir) == TRUE) {
                printf("already installed\n");
                continue;
            }
        } else {
            // TODO: what if it failed
            continue;
        }

        if ((status = git_clone(&repo, fullurl, fulldir, &opts))) {
            const git_error *err = git_error_last();
            if (err)
                fprintf(stderr, "error[%d]: %s\n", err->klass, err->message);
        }
    }
    return 0;
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
    {.name = "check",.func = &_pgm_chk},
    {.name = "inst",.func = &_pgm_inst},
    {.name = "list",.func = &_pgm_list},
};

int tman_cli_pgm(int argc, char **argv, struct tman_context *ctx)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    git_libgit2_init();
    for (int i = 0; i < ARRAY_SIZE(pgmcmds); ++i)
        if (strncmp(cmd, pgmcmds[i].name, CMDSIZ) == 0)
            return pgmcmds[i].func(argc - 1, argv + 1, ctx);
    return elog(1, "no such pgm command '%s'", cmd);
}
