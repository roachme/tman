#include <string.h>
#include <dirent.h>

#include "prj.h"
#include "cli.h"
#include "color.h"

static int tree_print_rec(struct tree *p)
{
    if (p != NULL) {
        tree_print_rec(p->left);
        printf(GRN "%c " CRESET BBLU "%-10s " CRESET "[%s] %s\n", p->mark, p->id, p->pgnout, p->desc);
        tree_print_rec(p->right);
    }
    return 0;
}

// TODO: Find a good error message in case option fails.  */
static int _prj_add(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    const char *errfmt = "cannot add project '%s': %s";
    int i, quiet, showhelp, status;
    struct tman_prj_add_opt opt;

    showhelp = quiet = FALSE;
    while ((c = getopt(argc, argv, ":hq")) != -1) {
        switch (c) {
            case 'h':
                showhelp = TRUE; break;
            case 'q':
                quiet = TRUE; break ;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp) {
        printf("prj add -h: under development\n");
        return 0;
    }

    if (optind == argc)
        return elog(1, "task prj required");

    for (i = optind; i < argc; ++i) {
        if ((status = tman_prj_add(ctx, argv[i], &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(1, errfmt, argv[i], tman_strerror());
        }
    }
    return status == TMAN_OK ? tman_pwd() : 1;
}

static int _prj_del(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    const char *errfmt = "cannot switch: %s";
    int i, quiet, showpath, showhelp, status;
    char *old_cprj = tman_prj_getcurr(NULL);
    struct tman_prj_del_opt opt;

    quiet = showhelp = showpath = FALSE;
    while ((c = getopt(argc, argv, ":hq")) != -1) {
        switch (c) {
            case 'h':
                showhelp = TRUE; break;
            case 'q':
                quiet = TRUE; break ;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp) {
        printf("prj del -h: under development\n");
        return 0;
    }

    i = optind;
    do {
        if ((status = tman_prj_del(ctx, argv[i], &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
        }
    } while (++i < argc);


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
    elog(1, "under development");
    return 0;
}

// roach: maybe it'll be useful
static int _prj_show(int argc, char **argv, tman_ctx_t *ctx)
{
    elog(1, "under development");
    return 0;
}

static int _prj_sync(int argc, char **argv, tman_ctx_t *ctx)
{
    int c, i, quiet, showhelp, status;
    char *prj;
    const char *errfmt = "cannot switch to '%s': %s";
    struct tman_prj_sync_opt opt = {
        .doswitch = TRUE,
    };

    quiet = showhelp = FALSE;
    while ((c = getopt(argc, argv, ":hnq")) != -1) {
        switch (c) {
            case 'h':
                showhelp = TRUE; break;
            case 'n':
                opt.doswitch = FALSE; break;
            case 'q':
                quiet = TRUE; break ;
            case ':':
                return elog(1, "option `-%c' requires an argument", optopt);
            default:
                return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp) {
        printf("prj sync -h: under development\n");
        return 0;
    }

    i = optind;
    do {
        if ((status = tman_prj_sync(ctx, argv[i], &opt)) != TMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
        }
    } while (++i < argc);

    return opt.doswitch && status == TMAN_OK ? tman_pwd() : status;
}

static const builtin_t prjcmds[] = {
    { .name = "add",  .func = &_prj_add  },
    { .name = "del",  .func = &_prj_del  },
    { .name = "list", .func = &_prj_list },
    { .name = "prev", .func = &_prj_prev },
    { .name = "set",  .func = &_prj_set  },
    { .name = "show", .func = &_prj_show }, // under consideration
    { .name = "sync", .func = &_prj_sync },
};

int tman_cli_prj(int argc, char **argv, tman_ctx_t *ctx)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < ARRAY_SIZE(prjcmds); ++i)
        if (strncmp(cmd, prjcmds[i].name, CMDSIZ) == 0)
            return prjcmds[i].func(argc - 1, argv + 1, ctx);

    return elog(1, "no such prj command '%s'", cmd);
}
