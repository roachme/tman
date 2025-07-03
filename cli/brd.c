#include <string.h>
#include <dirent.h>

#include "cli.h"

static int generate_units(tman_ctx_t * ctx, char *brd)
{
    struct tman_unit *units = NULL;
    char desc[100] = "autogenerate desciption for board ";

    strcat(desc, brd);
    units = tman_unit_add(units, "desc", desc);
    ctx->unitbrd = units;
    return 0;
}

// TODO: add support to generate board name
static int _brd_add(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    tman_arg_t args;
    const char *errfmt = "cannot add board '%s': %s";
    int i, quiet, showhelp, status;
    tman_opt_t opt = {
        .brd_switch = TRUE,
    };

    showhelp = quiet = FALSE;
    args.prj = args.brd = args.id = NULL;
    while ((c = getopt(argc, argv, ":hnp:q")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
            break;
        case 'n':
            opt.brd_switch = FALSE;
            break;
        case 'p':
            args.prj = optarg;
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
        return help_usage("brd-add");

    if (optind == argc)
        return elog(1, "board name required");

    for (i = optind; i < argc; ++i) {
        args.brd = argv[i];

        if (generate_units(ctx, args.brd)) {
            if (quiet == FALSE)
                elog(1, errfmt, args.prj, "unit generation failed");
            continue;
        } else if ((status = tman_brd_add(ctx, &args, &opt)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(1, errfmt, argv[i], tman_strerror());
            tman_unit_free(ctx, &args, NULL);
            continue;
        }
        // TODO: add hooks
        tman_unit_free(ctx, &args, NULL);
    }
    return status == LIBTMAN_OK ? tman_pwd() : 1;
}

static int _brd_del(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __FUNCTION__);
}

static int _brd_export(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __FUNCTION__);
}

static int _brd_import(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __FUNCTION__);
}

static int _brd_list(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __FUNCTION__);
}

static int _brd_prev(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __FUNCTION__);
}

static int _brd_move(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __FUNCTION__);
}

static int _brd_set(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __FUNCTION__);
}

// roach: maybe it'll be useful
static int _brd_show(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __FUNCTION__);
}

static int _brd_sync(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __FUNCTION__);
}

static const builtin_t brdcmds[] = {
    {.name = "add",.func = &_brd_add},
    {.name = "del",.func = &_brd_del},
    {.name = "export",.func = &_brd_export},    /* import tasks into board */
    {.name = "import",.func = &_brd_import},    /* export tasks from board */
    {.name = "list",.func = &_brd_list},
    {.name = "prev",.func = &_brd_prev},
    {.name = "move",.func = &_brd_move},
    {.name = "set",.func = &_brd_set},
    {.name = "show",.func = &_brd_show},
    {.name = "sync",.func = &_brd_sync},
};

int tman_cli_brd(int argc, char **argv, tman_ctx_t * ctx)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < ARRAY_SIZE(brdcmds); ++i)
        if (strncmp(cmd, brdcmds[i].name, CMDSIZ) == 0)
            return brdcmds[i].func(argc - 1, argv + 1, ctx);

    return elog(1, "no such brd command '%s'", cmd);
}
