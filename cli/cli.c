#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "cli.h"
#include "config.h"
#include "help.h"

#define NONEBOOL        -1      /* Not yet set boolean value */

struct config *tmancfg;

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

static int show_version()
{
    printf("%s: %s\n", PROGRAM, VERSION);
    return 0;
}

int tman_pwd_task(tman_arg_t * args)
{
    FILE *fp;

    if (tman_check_arg_prj(args) != LIBTMAN_OK)
        return emod_set(LIBTMAN_PRJ_NOCURR);
    if (tman_check_arg_brd(args) != LIBTMAN_OK)
        args->brd = "";
    if (tman_check_arg_task(args) != LIBTMAN_OK)
        args->task = "";

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
    FILE *fp;
    tman_arg_t args;

    args.prj = args.brd = args.task = NULL;

    if (tman_check_arg_prj(&args) != LIBTMAN_OK)
        return emod_set(LIBTMAN_PRJ_NOCURR);

    dlog(1, "tman_pwd_prj: prj: '%s'", args.prj);
    if ((fp = fopen(PWDFILE, "w")) == NULL)
        return 1;
    fprintf(fp, "%s/%s\n", tmancfg->base.task, args.prj);
    return fclose(fp);
}

int tman_pwd_board(void)
{
    FILE *fp;
    tman_arg_t args;

    args.prj = args.brd = args.task = NULL;

    if (tman_check_arg_prj(&args) != LIBTMAN_OK)
        return emod_set(LIBTMAN_PRJ_NOCURR);
    if (tman_check_arg_brd(&args) != LIBTMAN_OK)
        return emod_set(LIBTMAN_BRD_NOCURR);

    dlog(1, "tman_pwd_board: prj: '%s', brd: '%s'", args.prj, args.brd);
    if ((fp = fopen(PWDFILE, "w")) == NULL)
        return 1;
    fprintf(fp, "%s/%s/%s\n", tmancfg->base.task, args.prj, args.brd);
    return fclose(fp);
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
    {.name = "add",.func = &tman_cli_add,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "brd",.func = &tman_cli_brd,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "cfg",.func = &tman_cli_cfg,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "del",.func = &tman_cli_del,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "flow",.func = &tman_cli_flow,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "help",.func = &tman_cli_help,.setuplvl = LIBTMAN_SETUPSOFT},
    {.name = "init",.func = &tman_cli_init,.setuplvl = LIBTMAN_SETUPHARD},
    {.name = "list",.func = &tman_cli_list,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "move",.func = &tman_cli_move,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "prev",.func = &tman_cli_prev,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "prj",.func = &tman_cli_prj,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "set",.func = &tman_cli_set,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "show",.func = &tman_cli_show,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "sync",.func = &tman_cli_sync,.setuplvl = LIBTMAN_SETUPCHECK},
};

int main(int argc, char **argv)
{
    int usecolor, usedebug, usehooks;
    char *cmd = NULL, *option, *togfmt;
    tman_base_t base = {.task = NULL,.pgn = NULL };
    int c, i, status, cmdfound;
    tman_ctx_t *ctx;

    cmd = option = NULL;
    usecolor = usedebug = usehooks = NONEBOOL;
    togfmt = "option `-%c' accepts either 'on' or 'off'";

    /* Parse util itself options.  */
    while ((c = getopt(argc, argv, "+hC:D:F:H:P:T:V")) != -1) {
        switch (c) {
        case 'h':
            return help_lookup(NULL);
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
    tman_pwd_unset();

    if ((cmd = argv[i]) == NULL) {
        help_lookup(NULL);
        return 1;
    }

    if ((tmancfg = myconfig_create()) == NULL) {
        fprintf(stderr, "myconfig_create: FAIL\n");
    }

    if (tmancfg_parse(tmancfg)) {
        fprintf(stderr, "could parse config file\n");
        return 1;
    }

    ctx = NULL;
    cmdfound = FALSE;

    if (base.task != NULL) {
        free(tmancfg->base.task);
        tmancfg->base.task = strdup(base.task);
    }
    if (base.pgn != NULL) {
        free(tmancfg->base.pgn);
        tmancfg->base.pgn = strdup(base.pgn);
    }

    tmancfg->usehooks = usehooks != NONEBOOL ? usehooks : tmancfg->usehooks;
    tmancfg->usecolor = usecolor != NONEBOOL ? usecolor : tmancfg->usecolor;
    tmancfg->usedebug = usedebug != NONEBOOL ? usedebug : tmancfg->usedebug;

    if ((ctx = tman_init(&tmancfg->base)) == NULL) {
        elog(1, "could not init util: %s", tman_strerror());
        return 1;
    }

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
    if (cmdfound == FALSE && (status = ispgn(tmancfg->base.pgn, cmd)) == TRUE) {
        cmdfound = TRUE;
        if ((status = tman_setup(LIBTMAN_SETUPCHECK)) != LIBTMAN_OK) {
            elog(status, "setup failed: %s", tman_strerror());
            goto out;
        }
        status = tman_cli_plugin(argc - i, argv + i, ctx);
    }
    if (cmdfound == FALSE) {
        status = 1;
        elog(1, "%s: no such command or plugin", cmd);
    }

 out:
    ctx = tman_deinit(ctx);
    myconfig_destroy(tmancfg);
    return status;
}
