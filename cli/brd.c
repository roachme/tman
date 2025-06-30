#include <string.h>
#include <dirent.h>

#include "cli.h"

static int _brd_add(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __FUNCTION__);
}

static int _brd_del(int argc, char **argv, tman_ctx_t * ctx)
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
