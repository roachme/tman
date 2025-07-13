#include <stdio.h>
#include <string.h>

#include "cli.h"
#include "column.h"

static int _flow_list(int argc, char **argv, tman_ctx_t * ctx)
{
    // TODO: add option 'quiet'
    tman_arg_t args;
    int c, showhelp, verbose;

    showhelp = verbose = FALSE;
    args.prj = args.brd = args.task = NULL;
    while ((c = getopt(argc, argv, ":b:p:qv")) != -1) {
        switch (c) {
        case 'b':
            args.brd = optarg;
            break;
        case 'p':
            args.prj = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'v':
            verbose = TRUE;
            return elog(1, "option '-%c' under development", c);
        }
    }

    if (showhelp == TRUE)
        return help_usage("add");

    for (int i = 0; i < g_colname_size; ++i) {
        char mark = g_colname[i].mark;
        printf("%c %s \t\t%s\n", mark, g_colname[i].tag, g_colname[i].desc);
    }
    return 0;
}

static int _flow_next(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __func__);
}

static int _flow_prev(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "%s: under development", __func__);
}

static int _flow_move(int argc, char **argv, tman_ctx_t * ctx)
{
    tman_arg_t args;
    tman_opt_t opts;
    int c, i, quiet, status, showhelp, verbose;
    const char *errfmt = "cannot move '%s' to column '%s': %s";

    ctx->colname = "blog";      /* default column name is 'blog' */
    quiet = showhelp = verbose = FALSE;
    args.prj = args.brd = args.task = NULL;
    while ((c = getopt(argc, argv, ":c:hq")) != -1) {
        switch (c) {
        case 'c':
            ctx->colname = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'q':
            quiet = FALSE;
            break;
        }
    }

    if (showhelp)
        return help_usage("flow-move");

    i = optind;
    do {
        args.task = argv[i];
        if ((status = tman_task_flow(ctx, &args, &opts)) != LIBTMAN_OK) {
            if (quiet == FALSE) {
                args.task = args.task ? args.task : "NOCURR";
                elog(status, errfmt, args.task, ctx->colname, tman_strerror());
            }
        }
    } while (++i < argc);
    return status;
}

static const builtin_t cmds[] = {
    {.name = "list",.func = &_flow_list},       // NOTE: maybe rename it to 'show'
    {.name = "move",.func = &_flow_move},
    {.name = "next",.func = &_flow_next},
    {.name = "prev",.func = &_flow_prev},
};

int tman_cli_flow(int argc, char **argv, tman_ctx_t * ctx)
{
    char *cmd;

    // TODO: add command 'list' as default
    if ((cmd = argv[1]) == NULL)
        return elog(1, "no command specified");
    else if (strcmp(cmd, "-h") == 0 || strcmp(cmd, "help") == 0)
        return help_usage("flow");

    for (int i = 0; i < ARRAY_SIZE(cmds); ++i)
        if (strncmp(cmd, cmds[i].name, CMDSIZ) == 0)
            return cmds[i].func(argc - 1, argv + 1, ctx);
    return elog(1, "'%s': no such workflow command", cmd);
}
