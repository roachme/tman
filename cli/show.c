#include <string.h>

#include "cli.h"

/* TODO: Command 'add' generates them. Make sure they are in sync.  */
static char *unitkeys[] = { "prio", "type", "date", "desc", };

static unsigned int nunitkey = sizeof(unitkeys) / sizeof(unitkeys[0]);

static int valid_unitkeys(tman_unit_t * units)
{
    for (int i = 0; units && i < nunitkey; units = units->next) {
        if (strcmp(units->key, unitkeys[i]) != 0)
            return 1;
        ++i;
    }
    return 0;
}

static int show_key(tman_ctx_t * ctx, tman_arg_t * args, tman_unit_t * unitpgn,
                    char *key)
{
    struct tman_unit *units;

    if (strcmp(key, "id") == 0) {
        printf("%s\n", args->task);
        return 0;
    }

    for (units = ctx->units; units; units = units->next)
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

static int show_keys(tman_ctx_t * ctx, tman_arg_t * args, tman_unit_t * unitpgn)
{
    struct tman_unit *units;

    printf("%-7s : %s\n", "id", args->task);

    for (units = ctx->units; units; units = units->next)
        printf("%-7s : %s\n", units->key, units->val);

    for (; unitpgn; unitpgn = unitpgn->next)
        printf("%-7s : %s\n", unitpgn->key, unitpgn->val);

    return 0;
}

int tman_cli_show(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    char *key;
    tman_arg_t args;
    tman_unit_t *unitpgn;
    int i, quiet, showhelp, status;
    tman_opt_t opts = {.task_switch = TRUE };
    const char *errfmt = "cannot show units '%s': %s";

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

    i = optind;
    do {
        args.task = argv[i];

        if ((status = tman_task_get(ctx, &args, &opts)) != LIBTMAN_OK) {
            args.task = args.task ? args.task : "NOCURR";
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
            if ((status = show_key(ctx, &args, unitpgn, key)) && quiet == FALSE)
                elog(1, "cannot show key '%s': no such key", key);
        } else
            show_keys(ctx, &args, unitpgn);

        unitpgn = tman_unit_free(unitpgn);
        ctx->units = tman_unit_free(ctx->units);
    }
    while (++i < argc);

    return status;
}
