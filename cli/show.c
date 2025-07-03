#include <string.h>

#include "cli.h"

static int show_key(tman_ctx_t * ctx, tman_arg_t * args, char *key)
{
    struct tman_unit *unitbin, *unitpgn;

    if (strcmp(key, "id") == 0) {
        printf("%s\n", args->task);
        return LIBTMAN_OK;
    }

    for (unitbin = ctx->unitbin; unitbin; unitbin = unitbin->next)
        if (strcmp(key, unitbin->key) == 0) {
            printf("%s\n", unitbin->val);
            return LIBTMAN_OK;
        }

    for (unitpgn = ctx->unitpgn; unitpgn; unitpgn = unitpgn->next)
        if (strcmp(key, unitpgn->key) == 0) {
            printf("%s\n", unitpgn->val);
            return LIBTMAN_OK;
        }

    return 1;
}

static int pretty_show(tman_ctx_t * ctx, tman_arg_t * args, char *key)
{
    struct tman_unit *unitbin, *unitpgn;

    printf("%-7s : %s\n", "id", args->task);

    for (unitbin = ctx->unitbin; unitbin; unitbin = unitbin->next)
        printf("%-7s : %s\n", unitbin->key, unitbin->val);

    for (unitpgn = ctx->unitpgn; unitpgn; unitpgn = unitpgn->next)
        printf("%-7s : %s\n", unitpgn->key, unitpgn->val);

    return LIBTMAN_OK;
}

int tman_cli_show(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    char *key;
    tman_arg_t args;
    int i, quiet, showhelp, status;
    const char *errfmt = "cannot show units '%s': %s";

    key = NULL;
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

        if ((status = tman_task_show(ctx, &args, NULL)) != LIBTMAN_OK) {
            args.task = args.task ? args.task : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.task, tman_strerror());
            continue;
        } else if (hook_show(ctx, &args, "show")) {
            if (quiet == FALSE)
                elog(status, errfmt, args.task, "failed to execute hooks");
            continue;
        }

        if (key != NULL) {
            if (show_key(ctx, &args, key) != LIBTMAN_OK && quiet == FALSE) {
                if (quiet == FALSE)
                    elog(1, errfmt, args.task, "key not found");
            }
        } else {
            pretty_show(ctx, &args, key);
        }

        tman_unit_free(ctx, &args, NULL);
    } while (++i < argc);

    return status;
}
