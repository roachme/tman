#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "cli.h"
#include "config.h"
#include "help.h"

#define NONEBOOL        -1      /* Not yet set boolean value */

struct config *tmancfg;

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

int tman_pwd(void)
{
    FILE *fp;
    tman_arg_t args;

    if ((args.prj = project_getcurr()) == NULL) {
        dlog(1, "tman_pwd: no current project");
        return LIBTMAN_OK;
    }
    if ((args.id = task_curr(args.prj)) == NULL)
        args.id = "";

    dlog(1, "tman_pwd: prj = '%s', id = '%s'", args.prj, args.id);
    if ((fp = fopen(PWDFILE, "w")) == NULL)
        return 1;
    fprintf(fp, "%s/%s/%s\n", tmancfg->base.task, args.prj, args.id);
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
    {.name = "cfg",.func = &tman_cli_cfg,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "col",.func = &tman_cli_col,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "del",.func = &tman_cli_del,.setuplvl = LIBTMAN_SETUPCHECK},
    {.name = "fsck",.func = &tman_cli_fsck,.setuplvl = LIBTMAN_SETUPCHECK},
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
    int i, status, cmdfound;
    tman_ctx_t *ctx;

    cmd = option = NULL;
    usecolor = usedebug = usehooks = NONEBOOL;
    togfmt = "option `%s' accepts either 'on' or 'off'";

    /* Parse util itself options.  */
    for (i = 1; i < argc && argv[i][0] == '-'; ++i) {
        if ((option = argv[i]) && strcmp(option, "-T") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            base.task = argv[++i];
        } else if ((option = argv[i]) && strcmp(option, "-C") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            if (strcmp(argv[i + 1], "on") == 0)
                usecolor = TRUE;
            else if (strcmp(argv[i + 1], "off") == 0)
                usecolor = FALSE;
            else
                return elog(1, togfmt, option);
            ++i;                /* Skip option.  */
        } else if ((option = argv[i]) && strcmp(option, "-D") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", option);
            if (strcmp(argv[i + 1], "on") == 0)
                usedebug = TRUE;
            else if (strcmp(argv[i + 1], "off") == 0)
                usedebug = FALSE;
            else
                return elog(1, togfmt, option);
            ++i;                /* Skip option.  */
        } else if (strcmp(argv[i], "-h") == 0)
            return help_lookup(NULL);
        else if (strcmp(argv[i], "-P") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", argv[i]);
            base.pgn = argv[++i];
        } else if ((option = argv[i]) && strcmp(argv[i], "-H") == 0) {
            if (argv[i + 1] == NULL || argv[i + 1][0] == '-')
                return elog(1, "option `%s' requires an argument", argv[i]);
            if (strcmp(argv[i + 1], "on") == 0)
                usehooks = TRUE;
            else if (strcmp(argv[i + 1], "off") == 0)
                usehooks = FALSE;
            else
                return elog(1, togfmt, option);
            ++i;                /* Skip option.  */
        } else if (strcmp(argv[i], "-V") == 0)
            return show_version();
        else
            return elog(1, "invalid option `%s'", argv[i]);
    }

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
