#include <string.h>

#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

// TODO: add support to show multiple key values

static const char *errfmt = "cannot show units '%s': %s";

static int valid_unitkeys(tec_unit_t * units)
{
    for (int i = 0; units && i < nunitkey; units = units->next) {
        if (strcmp(units->key, unitkeys[i]) != 0)
            return 1;
        ++i;
    }
    return 0;
}

static int show_key(char *task, tec_unit_t * unitbin, tec_unit_t * unitpgn,
                    char *key)
{
    struct tec_unit *units;

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

static int show_keys(char *task, tec_unit_t * unitbin, tec_unit_t * unitpgn)
{
    struct tec_unit *units;
    const char *fmt = "%-" xstr(PADDING_UNIT) "s : %s\n";

    printf(fmt, "id", task);

    for (units = unitbin; units; units = units->next)
        printf(fmt, units->key, units->val);

    for (; unitpgn; unitpgn = unitpgn->next)
        printf(fmt, unitpgn->key, unitpgn->val);

    return 0;
}

int tec_cli_show(int argc, char **argv, tec_ctx_t * ctx)
{
    char c;
    char *key;
    tec_arg_t args;
    tec_unit_t *unitpgn;
    int i, quiet, showhelp, status;

    key = NULL;
    unitpgn = NULL;
    quiet = showhelp = FALSE;
    args.project = args.board = args.taskid = NULL;
    while ((c = getopt(argc, argv, ":b:hk:p:q")) != -1) {
        switch (c) {
        case 'b':
            args.board = optarg;
            break;
        case 'h':
            showhelp = TRUE;
            break;
        case 'k':
            key = optarg;
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
        return help_usage("show");

    if ((status = check_arg_project(&args, errfmt, quiet)))
        return status;
    else if ((status = check_arg_board(&args, errfmt, quiet)))
        return status;

    i = optind;
    do {
        args.taskid = argv[i];

        if ((status = check_arg_task(&args, errfmt, quiet)))
            continue;
        else if ((status = tec_task_get(teccfg.base.task, &args, ctx))) {
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, tec_strerror(status));
            continue;
        } else if (valid_unitkeys(ctx->units)) {
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, "invalid unit keys");
            continue;
        } else if (hook_show(&unitpgn, &args, "show")) {
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, "failed to execute hooks");
            continue;
        }

        if (key != NULL) {
            if ((status = show_key(args.taskid, ctx->units, unitpgn, key))
                && quiet == FALSE)
                elog(1, "cannot show key '%s': no such key", key);
        } else
            show_keys(args.taskid, ctx->units, unitpgn);

        unitpgn = tec_unit_free(unitpgn);
        ctx->units = tec_unit_free(ctx->units);
    }
    while (++i < argc);
    return status;
}
