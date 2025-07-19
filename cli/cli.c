#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "cli.h"
#include "aux/config.h"
#include "aux/toggle.h"

#define NONEBOOL        -1      /* Not yet set boolean value */

struct config *tmancfg;
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

static int valid_toggle(char *tog)
{
    if (strcmp(tog, "on") == 0)
        return TRUE;
    else if (strcmp(tog, "off") == 0)
        return FALSE;
    return -1;
}

static int ispgn(char *pgndir, const char *pgname)
{
    char path[PATHSIZ + 1];
    FILE *fp;

    sprintf(path, "%s/%s/%s", pgndir, pgname, pgname);

    if ((fp = fopen(path, "r")) == NULL)
        return FALSE;
    fclose(fp);
    return TRUE;
}

int tman_setup(int setuplvl)
{
    int status = LIBTMAN_OK;

    if (setuplvl == LIBTMAN_SETUP_SOFT) /* no filesystem check.  */
        ;
    else if (setuplvl == LIBTMAN_SETUP_HARD) {  /* check filesystem.  */
        status = tman_check_db();
    }
    return status;
}

static int show_version()
{
    printf("%s: %s\n", PROGRAM, VERSION);
    return 0;
}

int check_arg_prj(tman_arg_t * args)
{
    int status;

    if ((status = toggle_prj_get_curr(tmancfg->base.task, args))) {
        return status;
    } else if ((status = tman_check_arg_prj(args)))
        return status;
    return 0;
}

int get_column_index(char *colname)
{
    for (int i = 0; i < nbuiltin_column; ++i)
        if (strcmp(colname, builtin_columns[i].name) == 0)
            return i;
    return -1;
}

BOOL column_exist(const char *colname)
{
    for (int i = 0; i < nbuiltin_column; ++i)
        if (strcmp(colname, builtin_columns[i].name) == 0)
            return TRUE;
    // TODO: check user defined columns as well

    return FALSE;
}

tman_unit_t *generate_column(char *colname)
{
    unsigned int index;
    tman_unit_t *column;

    if ((index = get_column_index(colname)) == -1)
        return NULL;

    column = NULL;
    column = tman_unit_add(column, "mark", builtin_columns[index].mark);
    column = tman_unit_add(column, "name", builtin_columns[index].name);
    column = tman_unit_add(column, "prio", xstr(index));
    return column;
}

int tman_pwd_task(tman_arg_t * args)
{
    FILE *fp;
    int status;

    if ((status = toggle_prj_get_curr(tmancfg->base.task, args))) {
        return status;
    }
    if ((status = toggle_brd_get_curr(tmancfg->base.task, args))) {
        return status;
    }
    if ((status = toggle_task_get_curr(tmancfg->base.task, args))) {
        fprintf(stderr, "ererr: %s\n", tman_strerror());
        return status;
    }

    dlog(1, "tman_pwd: prj: '%s', brd: '%s', id: '%s'", args->prj, args->brd,
         args->task);
    if ((fp = fopen(PWDFILE, "w")) == NULL)
        return 1;
    fprintf(fp, "%s/%s/%s/%s\n", tmancfg->base.task, args->prj, args->brd,
            args->task);

    return fclose(fp);
}

int tman_pwd_project(void)
{
    return 1;
}

int tman_pwd_board(void)
{
    return 1;
}

int tman_pwd_unset(void)
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
    // TODO: add a debug log level
    if (tmancfg->usedebug == FALSE)
        return 0;

    va_list arg;
    va_start(arg, fmt);
    printf(PROGRAM ": ");
    vprintf(fmt, arg);
    printf("\n");
    va_end(arg);
    return 0;
}

static const builtin_t builtins[] = {
    {.name = "add",.func = &tman_cli_add,.setuplvl = LIBTMAN_SETUP_HARD},
    {.name = "board",.func = &tman_cli_board,.setuplvl = LIBTMAN_SETUP_HARD},
    {.name = "del",.func = &tman_cli_del,.setuplvl = LIBTMAN_SETUP_HARD},
    {.name = "help",.func = &tman_cli_help,.setuplvl = LIBTMAN_SETUP_SOFT},
    {.name = "init",.func = &tman_cli_init,.setuplvl = LIBTMAN_SETUP_SOFT},
    {.name = "list",.func = &tman_cli_list,.setuplvl = LIBTMAN_SETUP_HARD},
    {.name = "move",.func = &tman_cli_move,.setuplvl = LIBTMAN_SETUP_HARD},
    {.name = "prev",.func = &tman_cli_prev,.setuplvl = LIBTMAN_SETUP_HARD},
    {.name = "project",.func = &tman_cli_project,.setuplvl =
     LIBTMAN_SETUP_HARD},
    {.name = "set",.func = &tman_cli_set,.setuplvl = LIBTMAN_SETUP_HARD},
    {.name = "show",.func = &tman_cli_show,.setuplvl = LIBTMAN_SETUP_HARD},
    {.name = "switch",.func = &tman_cli_switch,.setuplvl = LIBTMAN_SETUP_HARD},
    {.name = "sync",.func = &tman_cli_sync,.setuplvl = LIBTMAN_SETUP_HARD},
};

int main(int argc, char **argv)
{
    tman_ctx_t *ctx;
    tman_base_t base;
    int c, i, status, cmdfound, showhelp;
    char *cmd, *option, *pgndir, *togfmt;
    int usecolor, usedebug, usehooks;

    ctx = NULL;
    cmdfound = showhelp = FALSE;
    usecolor = usedebug = usehooks = NONEBOOL;
    base.task = pgndir = cmd = option = NULL;
    togfmt = "option `-%c' accepts either 'on' or 'off'";

    /* Parse util itself options.  */
    while ((c = getopt(argc, argv, "+hC:D:F:H:P:T:V")) != -1) {
        switch (c) {
        case 'h':
            showhelp = TRUE;
        case 'C':
            if ((usecolor = valid_toggle(optarg)) == -1)
                return elog(1, togfmt, c);
            break;
        case 'D':
            if ((usedebug = valid_toggle(optarg)) == -1)
                return elog(1, togfmt, c);
            break;
        case 'F':
            return elog(1, "option `-%c' under development", c);
        case 'H':
            if ((usehooks = valid_toggle(optarg)) == -1)
                return elog(1, togfmt, c);
            break;
        case 'P':
            pgndir = optarg;
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
    tman_pwd_unset();

    if (showhelp == TRUE || (cmd = argv[i]) == NULL)
        return help_list_commands();

    if ((tmancfg = myconfig_create()) == NULL)
        return elog(1, "could not allocate memory for config");

    if (tmancfg_parse(tmancfg))
        return elog(1, "could parse config file");

    if (base.task != NULL) {
        free(tmancfg->base.task);
        tmancfg->base.task = strdup(base.task);
    }
    if (pgndir != NULL) {
        free(tmancfg->pgndir);
        tmancfg->pgndir = strdup(pgndir);
    }

    tmancfg->usehooks = usehooks != NONEBOOL ? usehooks : tmancfg->usehooks;
    tmancfg->usecolor = usecolor != NONEBOOL ? usecolor : tmancfg->usecolor;
    tmancfg->usedebug = usedebug != NONEBOOL ? usedebug : tmancfg->usedebug;

    if ((ctx = tman_init(&tmancfg->base)) == NULL)
        return elog(1, "could not init util: %s", tman_strerror());

    for (int idx = 0; idx < ARRAY_SIZE(builtins); ++idx)
        if (strcmp(cmd, builtins[idx].name) == 0) {
            cmdfound = TRUE;
            if ((status = tman_setup(builtins[idx].setuplvl)) != LIBTMAN_OK) {
                elog(status, "setup failed: %s", tman_strerror());
                goto out;
            }
            status = builtins[idx].func(argc - i, argv + i, ctx);
            goto out;
        }
    if (cmdfound == FALSE && (status = ispgn(tmancfg->pgndir, cmd)) == TRUE) {
        cmdfound = TRUE;
        if ((status = tman_setup(LIBTMAN_SETUP_HARD)) != LIBTMAN_OK) {
            elog(status, "setup failed: %s", tman_strerror());
            goto out;
        }
        status = tman_cli_plugin(argc - i, argv + i, ctx);
    }
    if (cmdfound == FALSE) {
        status = 1;
        elog(1, "'%s': no such command or plugin", cmd);
    }

 out:
    ctx = tman_deinit(ctx);
    myconfig_destroy(tmancfg);
    return status;
}
