#include <ctype.h>
#include <string.h>
#include <dirent.h>

#include "cli.h"

static int generate_units(struct tman_context *ctx, char *prj)
{
    struct tman_unit *units = NULL;
    char desc[100] = "autogenerate desciption for project ";

    strcat(desc, prj);
    units = tman_unit_add(units, "desc", desc);
    ctx->unitbin = units;
    return 0;
}

static int valid_desc(const char *val)
{
    if (!isalnum(*val++))
        return FALSE;
    for (; *val; ++val)
        if (!(isalnum(*val) || isspace(*val) || *val == '_' || *val == '-'))
            return FALSE;
    return isalnum(*--val) != 0;
}

static int tree_print_rec(struct tree *p)
{
    if (p != NULL) {
        tree_print_rec(p->left);
        LIST_PRJ_UNITS(p);
        tree_print_rec(p->right);
    }
    return 0;
}

// TODO: Find a good error message in case option fails.  */
static int _prj_add(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    struct tman_arg args;
    const char *errfmt = "cannot add project '%s': %s";
    int i, quiet, showhelp, status;
    struct tman_option opt = {
        .prj_switch = TRUE,
    };

    showhelp = quiet = FALSE;
    args.prj = args.id = NULL;
    while ((c = getopt(argc, argv, ":hnq")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            opt.prj_switch = FALSE;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp)
        return help_usage("prj-add");

    if (optind == argc)
        return elog(1, "project name required");

    tman_pwd_unset();
    for (i = optind; i < argc; ++i) {
        args.prj = argv[i];

        if (generate_units(ctx, args.prj)) {
            if (quiet == FALSE)
                elog(1, errfmt, args.prj, "unit generation failed");
            continue;
        } else if ((status = tman_prj_add(ctx, &args, &opt)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(1, errfmt, argv[i], tman_strerror());
            continue;
        }
    }
    return status == LIBTMAN_OK ? tman_pwd() : 1;
}

static int _prj_del(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    struct tman_arg args;
    const char *errfmt = "cannot switch: %s";
    int i, choice, quiet, showhelp, showprompt, status;
    struct tman_option opt;

    showprompt = TRUE;
    args.prj = args.id = NULL;
    choice = quiet = showhelp = FALSE;
    while ((c = getopt(argc, argv, ":hnq")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            showprompt = FALSE;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp)
        return help_usage("prj-del");
    else if (showprompt) {
        /* Hotfix: if you add to stdout, than it'll be catched in my shell.  */
        fprintf(stderr, "Are you sure to delete projects(s)? [y/N] ");
        choice = getchar();
        if (choice != 'y' && choice != 'Y') {
            return 0;
        }
    }

    tman_pwd_unset();
    i = optind;
    do {
        args.prj = argv[i];
        if ((status = tman_prj_del(ctx, &args, &opt)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
        }
    } while (++i < argc);

    dlog(1, "prj_del: status = %d", status);

    // TODO: update current directory if current prj got deleted.
    return status == LIBTMAN_OK ? tman_pwd() : status;
}

static int _prj_list(int argc, char **argv, struct tman_context *ctx)
{
    int status;

    if ((status = tman_prj_list(ctx, NULL)) != LIBTMAN_OK) {
        elog(1, "_prj_list: something went wrong");
        return status;
    }
    return tree_print_rec(ctx->prjs);
}

static int _prj_prev(int argc, char **argv, struct tman_context *ctx)
{
    int status;
    struct tman_option opt;

    tman_pwd_unset();
    if ((status = tman_prj_prev(ctx, &opt)) != LIBTMAN_OK)
        return elog(status, "cannot switch: %s", tman_strerror());
    return tman_pwd();
}

static int _prj_rename(int argc, char **argv, struct tman_context *ctx)
{
    int c, quiet, showhelp, status;
    struct tman_arg src, dst;

    quiet = showhelp = FALSE;
    src.id = src.brd = src.prj = NULL;
    dst.id = dst.brd = dst.prj = NULL;

    while ((c = getopt(argc, argv, ":hq")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'q':
            quiet = TRUE;
            break;
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp)
        return help_usage("prj-rename");

    if (argc - optind != 2)
        return elog(1, "source or destination project name missing");

    src.prj = argv[optind];
    dst.prj = argv[optind + 1];

    /* TODO: trigger hooks if any */

    tman_pwd_unset();
    if ((status = tman_prj_rename(NULL, &src, &dst))) {
        if (quiet == FALSE)
            elog(status, "could not rename project: %s", tman_strerror());
    }
    return tman_pwd();
}

static int _prj_set(int argc, char **argv, struct tman_context *ctx)
{
    struct tman_arg args;
    int atleast_one_key_set;
    int c, i, quiet, showhelp;
    const char *errfmt = "could not set project unit value '%s': %s";

    quiet = showhelp = FALSE;
    atleast_one_key_set = FALSE;
    args.id = args.brd = args.prj = NULL;
    while ((c = getopt(argc, argv, ":d:hq")) != -1) {
        switch (c) {
        case 'd':
            if (valid_desc(optarg) == FALSE) {
                elog(1, "invalid description '%s'", optarg);
                help_usage("prj-set");
                return 1;
            }
            atleast_one_key_set = TRUE;
            ctx->unitbin = tman_unit_add(ctx->unitbin, "desc", optarg);
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp)
        return help_usage("prj-set");
    if (atleast_one_key_set == FALSE) {
        elog(1, "gotta supply one of the options");
        help_usage("prj-set");
        return 1;
    }

    i = optind;
    do {
        int status;
        args.prj = argv[i];

        if ((status =
             tman_prj_set(ctx, &args, ctx->unitbin, NULL)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
        }
    } while (++i < argc);

    ctx->unitbin = tman_unit_free(ctx, &args, NULL);
    return 0;
}

// roach: maybe it'll be useful
static int _prj_show(int argc, char **argv, struct tman_context *ctx)
{
    struct tman_arg args;
    int c, i, quiet, showhelp, status;
    struct tman_unit *unitbin, *unitpgn;
    const char *errfmt = "cannot show project units '%s': %s";

    unitbin = unitpgn = NULL;
    quiet = showhelp = FALSE;
    args.prj = args.id = NULL;
    while ((c = getopt(argc, argv, ":hq")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp)
        return help_usage("prj-show");

    i = optind;
    do {
        args.prj = argv[i];
        if ((status = tman_prj_show(ctx, &args, NULL)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
            continue;
        }

        printf("%-7s : %s\n", "prj", args.prj);
        for (unitbin = ctx->unitbin; unitbin; unitbin = unitbin->next)
            printf("%-7s : %s\n", unitbin->key, unitbin->val);

        // TODO: add plugin output
    } while (++i < argc);

    return status;
}

static int _prj_sync(int argc, char **argv, struct tman_context *ctx)
{
    struct tman_arg args;
    int c, i, quiet, showhelp, status;
    const char *errfmt = "cannot switch to '%s': %s";
    struct tman_option opt = {
        .prj_switch = TRUE,
    };

    quiet = showhelp = FALSE;
    args.prj = args.id = NULL;
    while ((c = getopt(argc, argv, ":hnq")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            opt.prj_switch = FALSE;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (showhelp)
        return help_usage("prj-sync");

    tman_pwd_unset();
    i = optind;
    do {
        args.prj = argv[i];
        if ((status = tman_prj_sync(ctx, &args, &opt)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, argv[i], tman_strerror());
        }
    } while (++i < argc);

    return opt.prj_switch && status == LIBTMAN_OK ? tman_pwd() : status;
}

static const builtin_t prjcmds[] = {
    {.name = "add",.func = &_prj_add},
    {.name = "del",.func = &_prj_del},
    {.name = "list",.func = &_prj_list},
    {.name = "prev",.func = &_prj_prev},
    {.name = "rename",.func = &_prj_rename},
    {.name = "set",.func = &_prj_set},
    {.name = "show",.func = &_prj_show},
    {.name = "sync",.func = &_prj_sync},
};

int tman_cli_prj(int argc, char **argv, struct tman_context *ctx)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < ARRAY_SIZE(prjcmds); ++i)
        if (strncmp(cmd, prjcmds[i].name, CMDSIZ) == 0)
            return prjcmds[i].func(argc - 1, argv + 1, ctx);

    return elog(1, "no such prj command '%s'", cmd);
}
