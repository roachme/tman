#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>

#include "cli.h"
#include "aux/config.h"
#include "aux/toggle.h"

struct config teccfg;
char *unitkeys[] = { "prio", "type", "date", "desc", };

unsigned int nunitkey = sizeof(unitkeys) / sizeof(unitkeys[0]);

// TODO: add support to rename default column names in config file
column_t builtin_columns[] = {
    {.prio = FIRST_COLUMN,.mark = "+",.name = "todo",},
    {.prio = FIRST_COLUMN + 1,.mark = ">",.name = "test",},
    {.prio = LAST_COLUMN,.mark = "-",.name = "done",},
};

unsigned int nbuiltin_column =
    sizeof(builtin_columns) / sizeof(builtin_columns[0]);

static builtin_t builtins[] = {
    {.name = "add",.func = &tec_cli_add,.option = TEC_SETUP_HARD},
    {.name = "board",.func = &tec_cli_board,.option = TEC_SETUP_HARD},
    {.name = "column",.func = &tec_cli_column,.option = TEC_SETUP_HARD},
    {.name = "del",.func = &tec_cli_del,.option = TEC_SETUP_HARD},
    {.name = "help",.func = &tec_cli_help,.option = TEC_SETUP_SOFT},
    {.name = "init",.func = &tec_cli_init,.option = TEC_SETUP_SOFT},
    {.name = "list",.func = &tec_cli_list,.option = TEC_SETUP_HARD},
    {.name = "move",.func = &tec_cli_move,.option = TEC_SETUP_HARD},
    {.name = "prev",.func = &tec_cli_prev,.option = TEC_SETUP_HARD},
    {.name = "project",.func = &tec_cli_project,.option = TEC_SETUP_HARD},
    {.name = "set",.func = &tec_cli_set,.option = TEC_SETUP_HARD},
    {.name = "show",.func = &tec_cli_show,.option = TEC_SETUP_HARD},
    {.name = "sync",.func = &tec_cli_sync,.option = TEC_SETUP_HARD},
};

static int tec_setup(int setuplvl)
{
    int status = LIBTEC_OK;

    if (setuplvl == TEC_SETUP_SOFT)     /* no filesystem check.  */
        ;
    else if (setuplvl == TEC_SETUP_HARD) {      /* check filesystem.  */
        status = tec_check_db(teccfg.base.task);
    }
    return status;
}

static builtin_t *is_builtin(char *cmd)
{
    for (int idx = 0; idx < ARRAY_SIZE(builtins); ++idx)
        if (strcmp(cmd, builtins[idx].name) == 0)
            return &builtins[idx];
    return NULL;
}

static int is_plugin(char *pgndir, const char *pgname)
{
    FILE *fp;
    char path[PATH_MAX + 1];

    sprintf(path, "%s/%s/%s", pgndir, pgname, pgname);

    if ((fp = fopen(path, "r")) == NULL)
        return false;
    fclose(fp);
    return true;
}

static int run_builtin(int argc, char **argv, builtin_t * cmd)
{
    int status;
    tec_ctx_t ctx = CTX_INIT;

    if ((status = tec_setup(cmd->option)) != LIBTEC_OK)
        elog(status, "setup failed: %s", tec_strerror(status));
    else
        status = cmd->func(argc, argv, &ctx);

    tec_config_destroy(&teccfg);
    return status;
}

static int run_plugin(int argc, char **argv)
{
    int status;
    tec_ctx_t ctx = CTX_INIT;

    if ((status = tec_setup(TEC_SETUP_HARD)) != LIBTEC_OK)
        elog(status, "setup failed: %s", tec_strerror(status));
    else
        status = tec_cli_plugin(argc, argv, &ctx);

    tec_config_destroy(&teccfg);
    return status;
}

static int valid_toggle(char *tog)
{
    if (strcmp(tog, "on") == 0)
        return true;
    else if (strcmp(tog, "off") == 0)
        return false;
    return -1;
}

static int show_version()
{
    printf("%s: %s\n", PROGRAM, VERSION);
    return 0;
}

int is_valid_length(const char *obj, int len)
{
    if (strlen(obj) <= len)
        return true;
    return false;
}

int check_arg_project(tec_arg_t * args, const char *errfmt, int quiet)
{
    int status;

    if ((status = toggle_project_get_curr(teccfg.base.task, args))) {
        if (quiet == false)
            elog(status, errfmt, "NOCURR", "no current project");
        return status;
    } else if ((status = tec_project_valid(teccfg.base.task, args))) {
        if (quiet == false)
            elog(status, errfmt, args->project, tec_strerror(status));
        return status;
    } else if (is_valid_length(args->project, PRJSIZ) == false) {
        status = 1;
        if (quiet == false)
            elog(status, errfmt, args->project, "project name is too long");
        return status;
    } else if ((status = tec_project_exist(teccfg.base.task, args))) {
        if (quiet == false)
            elog(status, errfmt, args->project, tec_strerror(status));
        return status;
    }
    return 0;
}

int check_arg_board(tec_arg_t * args, const char *errfmt, int quiet)
{
    int status;

    if ((status = toggle_board_get_curr(teccfg.base.task, args))) {
        if (quiet == false)
            elog(status, errfmt, "NOCURR", "no current board");
        return status;
    } else if ((status = tec_board_valid(teccfg.base.task, args))) {
        if (quiet == false)
            elog(status, errfmt, args->board, tec_strerror(status));
        return status;
    } else if (is_valid_length(args->board, BRDSIZ) == false) {
        status = 1;
        if (quiet == false)
            elog(status, errfmt, args->board, "board name is too long");
        return status;
    } else if ((status = tec_board_exist(teccfg.base.task, args))) {
        if (quiet == false)
            elog(status, errfmt, args->board, tec_strerror(status));
        return status;
    }
    return 0;
}

int check_arg_task(tec_arg_t * args, const char *errfmt, int quiet)
{
    int status;

    if ((status = toggle_task_get_curr(teccfg.base.task, args))) {
        if (quiet == false)
            elog(status, errfmt, "NOCURR", "no current task");
        return status;
    } else if ((status = tec_task_valid(teccfg.base.task, args))) {
        if (quiet == false)
            elog(status, errfmt, args->taskid, tec_strerror(status));
        return status;
    } else if (is_valid_length(args->taskid, IDSIZ) == false) {
        status = 1;
        if (quiet == false)
            elog(status, errfmt, args->taskid, "task ID is too long");
        return status;
    } else if ((status = tec_task_exist(teccfg.base.task, args))) {
        if (quiet == false)
            elog(status, errfmt, args->taskid, tec_strerror(status));
        return status;
    }
    return 0;
}

int get_column_index(char *colname)
{
    for (int i = 0; i < nbuiltin_column; ++i)
        if (strcmp(colname, builtin_columns[i].name) == 0)
            return i;
    return -1;
}

bool column_exist(const char *colname)
{
    for (int i = 0; i < nbuiltin_column; ++i)
        if (strcmp(colname, builtin_columns[i].name) == 0)
            return true;
    // TODO: check user defined columns as well

    return false;
}

tec_unit_t *generate_column(char *colname)
{
    unsigned int index;
    tec_unit_t *column;

    if ((index = get_column_index(colname)) == -1)
        return NULL;

    column = NULL;
    column = tec_unit_add(column, "name", builtin_columns[index].name);
    return column;
}

int tec_pwd_project(tec_arg_t * args)
{
    FILE *fp;
    char *taskdir = teccfg.base.task;

    if ((fp = fopen(PWDFILE, "w"))) {
        const char *fmt = "%s/%s\n";
        const char *debug_fmt = "PWD project: '%s'";

        dlog(1, debug_fmt, args->project);
        fprintf(fp, fmt, taskdir, args->project);
        return fclose(fp);
    }
    return 1;

}

int tec_pwd_board(tec_arg_t * args)
{
    FILE *fp;
    char *taskdir = teccfg.base.task;

    if ((fp = fopen(PWDFILE, "w"))) {
        const char *fmt = "%s/%s/%s\n";
        const char *debug_fmt = "PWD project: '%s', board: '%s'";

        dlog(1, debug_fmt, args->project, args->board);
        fprintf(fp, fmt, taskdir, args->project, args->board);
        return fclose(fp);
    }
    return 1;
}

int tec_pwd_task(tec_arg_t * args)
{
    FILE *fp;
    char *taskdir = teccfg.base.task;

    if ((fp = fopen(PWDFILE, "w"))) {
        const char *fmt = "%s/%s/%s/%s\n";
        const char *debug_fmt = "PWD project: '%s', board: '%s', task ID: '%s'";

        dlog(1, debug_fmt, args->project, args->board, args->taskid);
        fprintf(fp, fmt, taskdir, args->project, args->board, args->taskid);
        return fclose(fp);
    }
    return 1;
}

int tec_pwd_unset(void)
{
    FILE *fp;

    if ((fp = fopen(PWDFILE, "w")) == NULL)
        return 1;
    return fclose(fp);
}

int elog(int status, const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    fprintf(stderr, PROGRAM ": ");
    vfprintf(stderr, fmt, arg);
    fprintf(stderr, "\n");
    va_end(arg);
    return status;
}

int dlog(int level, const char *fmt, ...)
{
    if (teccfg.opts.debug == false)
        return 0;

    va_list arg;
    va_start(arg, fmt);
    printf(PROGRAM ": ");
    vprintf(fmt, arg);
    printf("\n");
    va_end(arg);
    return 0;
}

int main(int argc, char **argv)
{
    tec_opt_t opts;
    tec_base_t base;
    builtin_t *builtin;
    int c, i, showhelp;
    char *cmd, *option, *togfmt;

    showhelp = false;
    opts.color = opts.debug = opts.hook = NONEBOOL;
    base.pgn = base.task = cmd = option = NULL;
    togfmt = "option `-%c' accepts either 'on' or 'off'";

    /* Parse util itself options.  */
    while ((c = getopt(argc, argv, "+:hC:D:F:H:P:T:V")) != -1) {
        switch (c) {
        case 'h':
            showhelp = true;
        case 'C':
            if ((opts.color = valid_toggle(optarg)) == -1)
                return elog(1, togfmt, c);
            break;
        case 'D':
            if ((opts.debug = valid_toggle(optarg)) == -1)
                return elog(1, togfmt, c);
            break;
        case 'F':
            return elog(1, "option `-%c' under development", c);
        case 'H':
            if ((opts.hook = valid_toggle(optarg)) == -1)
                return elog(1, togfmt, c);
            break;
        case 'P':
            base.pgn = optarg;
            break;
        case 'T':
            base.task = optarg;
            break;
        case 'V':
            return show_version();
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    i = optind;
    optind = 0;                 /* Unset option index cuz subcommands use getopt too.  */
    tec_pwd_unset();

    if (showhelp == true || (cmd = argv[i]) == NULL)
        cmd = "help";

    if (tec_config_init(&teccfg))
        return elog(1, "could init config file");
    else if (tec_config_parse(&teccfg))
        return elog(1, "could parse config file");
    else if (tec_config_set_base(&base))
        return elog(1, "could set config base directories");
    else if (tec_config_set_options(&opts))
        return elog(1, "could set config options");

    if ((builtin = is_builtin(cmd)) != NULL)
        return run_builtin(argc - i, argv + i, builtin);
    else if (is_plugin(teccfg.base.pgn, cmd) == true)
        return run_plugin(argc - i, argv + i);
    return elog(1, "'%s': no such command or plugin", cmd);
}
