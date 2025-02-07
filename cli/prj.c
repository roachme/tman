#include <string.h>
#include <dirent.h>

#include "prj.h"
#include "cli.h"

static int tree_print_rec(struct tree *p)
{
    if (p != NULL) {
        tree_print_rec(p->left);
        printf("%c %-10s [%s] %s\n", p->mark, p->id, p->pgnout, p->desc);
        tree_print_rec(p->right);
    }
    return 0;
}

// TODO: Find a good error message in case option fails.  */
static int _prj_add(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    int i, status;
    struct tman_prj_add_opt opt;

    while ((c = getopt(argc, argv, ":")) != -1) {
        switch (c) {
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (optind == argc)
        return elog(1, "task prj required");

    for (i = optind; i < argc; ++i)
        status = tman_prj_add(ctx, argv[i], &opt);
    return status == TMAN_OK ? tman_pwd() : 1;
}

// roach: maybe it'll be useful
static int _prj_show(int argc, char **argv, tman_ctx_t *ctx)
{
    elog(1, "under development");
    return 0;
}

static int _prj_del(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    int status;
    int showpath = FALSE;
    char *old_cprj = tman_prj_getcurr(NULL);
    struct tman_prj_del_opt opt;

    while ((c = getopt(argc, argv, ":")) != -1) {
        switch (c) {
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    for (int i = optind; i < argc; ++i)
        status = tman_prj_del(ctx, argv[i], &opt);
    if (optind == argc) /* delete current task id */
        status = tman_prj_del(ctx, NULL, &opt);

    // TODO: update current directory if current prj got deleted.
    if (strcpy(old_cprj, tman_prj_getcurr(NULL))) {
        showpath = TRUE;
    }

    return status == TMAN_OK && showpath == TRUE ? tman_pwd() : 1;
}

static int _prj_list(int argc, char **argv, tman_ctx_t *ctx)
{
    int status;

    if ((status = tman_prj_list(ctx, NULL)) != TMAN_OK) {
        elog(1, "_prj_list: something went wrong");
        return status;
    }
    return tree_print_rec(ctx->prjs);
}

static int _prj_prev(int argc, char **argv, tman_ctx_t *ctx)
{
    int status;
    struct tman_prj_prev_opt opt;
    const char *errfmt = "cannot switch: %s";

    if ((status = tman_prj_prev(ctx, &opt)) != TMAN_OK)
        return elog(status, errfmt, tman_strerror());
    return tman_pwd();
}

static int _prj_set(int argc, char **argv, tman_ctx_t *ctx)
{
    return 0;
}

static int _prj_use(int argc, char **argv, tman_ctx_t *ctx)
{
    int status;
    char *prj;
    const char *errfmt = "cannot switch to '%s': %s";
    struct tman_prj_use_opt opt;

    // TODO: throws error if switch to current prj
    if ((prj = argv[1]) == NULL)
        return elog(1, errfmt, "NOENV", "project name required");
    else if ((status = tman_prj_use(ctx, prj, &opt)) != TMAN_OK)
        return elog(status, errfmt, prj, tman_strerror());
    return tman_pwd();
}

static const builtin_t prjcmds[] = {
    { .name = "add",  .func = &_prj_add  },
    { .name = "del",  .func = &_prj_del  },
    { .name = "list", .func = &_prj_list },
    { .name = "prev", .func = &_prj_prev },
    { .name = "set",  .func = &_prj_set  },
    { .name = "show", .func = &_prj_show }, // under consideration
    { .name = "use",  .func = &_prj_use  },
};

int tman_cli_prj(int argc, char **argv, tman_ctx_t *ctx)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < ARRAY_SIZE(prjcmds); ++i)
        if (strncmp(cmd, prjcmds[i].name, CMDSIZ) == 0)
            return prjcmds[i].func(argc - 1, argv + 1, ctx);

    return elog(1, "no such prj command '%s'", cmd);
}
