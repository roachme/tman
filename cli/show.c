#include <string.h>

#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

// TODO: add support to show multiple key values

static const char *errfmt = "cannot show units '%s': %s";

static int valid_unitkeys(tman_unit_t * units)
{
    for (int i = 0; units && i < nunitkey; units = units->next) {
        if (strcmp(units->key, unitkeys[i]) != 0)
            return 1;
        ++i;
    }
    return 0;
}

static int show_key(char *task, tman_unit_t * unitbin, tman_unit_t * unitpgn,
                    char *key)
{
    struct tman_unit *units;

    if (strcmp(key, "id") == 0) {
        printf("%s\n", task);
        return 0;
    }

    for (units = unitbin; units; units = units->next)
        if (strcmp(key, units->key) == 0) {
            printf("%s\n", units->val);
            return 0;
        }

    for (; unitpgn; unitpgn = unitpgn->next)
        if (strcmp(key, unitpgn->key) == 0) {
            printf("%s\n", unitpgn->val);
            return 0;
        }

    return 1;
}

static int show_keys(char *task, tman_unit_t * unitbin, tman_unit_t * unitpgn)
{
    struct tman_unit *units;
    const char *fmt = "%-" xstr(PADDING_UNIT) "s : %s\n";

    printf(fmt, "id", task);

    for (units = unitbin; units; units = units->next)
        printf(fmt, units->key, units->val);

    for (; unitpgn; unitpgn = unitpgn->next)
        printf(fmt, unitpgn->key, unitpgn->val);

    return 0;
}

int tman_cli_show(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    char *key;
    tman_arg_t args;
    tman_unit_t *unitpgn;
    int i, quiet, showhelp, status;

    key = NULL;
    unitpgn = NULL;
    quiet = showhelp = FALSE;
    args.prj = args.brd = args.task = NULL;
    while ((c = getopt(argc, argv, ":b:hk:p:q")) != -1) {
        switch (c) {
        case 'b':
            args.brd = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'k':
            key = optarg;
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

    if (showhelp == TRUE)
        return help_usage("show");

    if ((status = toggle_prj_get_curr(tmancfg->base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current project");
        return status;
    } else if ((status = toggle_brd_get_curr(tmancfg->base.task, &args))) {
        if (quiet == FALSE)
            elog(status, errfmt, "NOCURR", "no current board");
        return status;
    }

    i = optind;
    do {
        args.task = argv[i];

        if ((status = toggle_task_get_curr(tmancfg->base.task, &args))) {
            if (quiet == FALSE)
                elog(status, errfmt, "NOCURR", "no current task");
            return status;
        } else if ((status = tman_task_get(ctx, &args)) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, args.task, tman_strerror());
            continue;
        } else if (valid_unitkeys(ctx->units)) {
            if (quiet == FALSE)
                elog(status, errfmt, args.task, "invalid unit keys");
            continue;
        } else if (hook_show(&unitpgn, &args, "show")) {
            if (quiet == FALSE)
                elog(status, errfmt, args.task, "failed to execute hooks");
            continue;
        }

        if (key != NULL) {
            if ((status = show_key(args.task, ctx->units, unitpgn, key))
                && quiet == FALSE)
                elog(1, "cannot show key '%s': no such key", key);
        } else
            show_keys(args.task, ctx->units, unitpgn);

        unitpgn = tman_unit_free(unitpgn);
        ctx->units = tman_unit_free(ctx->units);
    }
    while (++i < argc);

    return status;
}
