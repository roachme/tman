#include <string.h>

#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

static int _column_list(int argc, char **argv, tman_ctx_t * ctx)
{
    for (int i = 0; i < nbuiltin_column; ++i) {
        column_t columns = builtin_columns[i];
        printf("%s %s %d\n", columns.mark, columns.name, columns.prio);
    }

    // TODO: show user define columns
    return 0;
}

static int _column_move(int argc, char **argv, tman_ctx_t * ctx)
{
    tman_arg_t args;
    char c, *errfmt, *colname;
    int i, quiet, showhelp, status;

    colname = "todo";           /* Default column to move task to.  */
    quiet = showhelp = FALSE;
    errfmt = "cannot move task to column '%s': %s";
    args.project = args.board = args.taskid = NULL;
    while ((c = getopt(argc, argv, ":b:c:hp:q")) != -1) {
        switch (c) {
        case 'b':
            args.board = optarg;
            break;
        case 'c':
            colname = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'p':
            args.project = optarg;
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
    if (showhelp == TRUE)
        return help_usage("column-move");

    if ((status = toggle_project_get_curr(tmancfg.base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current project");
        return status;
    } else if ((status = toggle_board_get_curr(tmancfg.base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current board");
        return status;
    } else if (column_exist(colname) == FALSE) {
        if (quiet == FALSE)
            elog(1, "'%s': no such column", colname);
        return 1;
    } else if ((ctx->column = generate_column(colname)) == NULL) {
        if (quiet == FALSE)
            elog(1, "could not generate column");
        return 1;
    }

    i = optind;
    do {
        args.taskid = argv[i];

        if ((status = toggle_task_get_curr(tmancfg.base.task, &args))) {
            if (quiet == FALSE)
                elog(status, errfmt, "NOCURR", "no current task");
            return status;
        } else if (colname
                   && (status =
                       tman_task_column_set(tmancfg.base.task, &args, ctx))) {
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, tman_strerror(status));
            continue;
        } else if (hook_action(&args, "column-move")) {
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, "failed to execute hooks");
            continue;
        }
    } while (++i < argc);
    return status;
}

static int _column_rename(int argc, char **argv, tman_ctx_t * ctx)
{
    return elog(1, "under development: rename column in file .tman/column");
}

static const builtin_t column_commands[] = {
    {.name = "list",.func = &_column_list},
    {.name = "move",.func = &_column_move},
    {.name = "rename",.func = &_column_rename},
};

int tman_cli_column(int argc, char **argv, tman_ctx_t * ctx)
{
    char *cmd = argv[1] != NULL ? argv[1] : "list";

    for (int i = 0; i < ARRAY_SIZE(column_commands); ++i)
        if (strcmp(cmd, column_commands[i].name) == 0)
            return column_commands[i].func(argc - 1, argv + 1, ctx);

    return elog(1, "'%s': no such column command", cmd);
}
