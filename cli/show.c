#include <string.h>

#include "show.h"
#include "cli.h"
#include "config.h"

static const char *errfmt = "cannot show units '%s': %s";

static int show_key(struct tman_context *ctx, char *key)
{
    int i;
    struct unit *unitbin, *unitpgn;

    if (strcmp(key, "id") == 0) {
        printf("%s\n", ctx->units.id);
        return TMAN_OK;
    }

    for (unitbin = ctx->units.bin, i = 0; i < NKEYS; ++i)
        if (strcmp(key, unitbin[i].key) == 0) {
            printf("%s\n", unitbin[i].val);
            return TMAN_OK;
        }

    for (unitpgn = ctx->units.pgn; unitpgn != NULL; unitpgn = unitpgn->next)
        if (strcmp(key, unitpgn->key) == 0) {
            printf("%s\n", unitpgn->val);
            return TMAN_OK;
        }

    return 1;
}

static int pretty_show(struct tman_context *ctx, char *key)
{
    int i;
    struct unit *unitbin, *unitpgn;

    printf("%-7s : %s\n", "id", ctx->units.id);

    for (unitbin = ctx->units.bin, i = 0; i < NKEYS; ++i)
        printf("%-7s : %s\n", unitbin[i].key, unitbin[i].val);

    for (unitpgn = ctx->units.pgn; unitpgn != NULL; unitpgn = unitpgn->next)
        printf("%-7s : %s\n", unitpgn->key, unitpgn->val);

    return TMAN_OK;
}

int tman_cli_show(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    char *key;
    int i, quiet, showhelp, status;
    struct tman_arg args;

    quiet = showhelp = FALSE;
    key = args.prj = args.id = args.brd = NULL;
    while ((c = getopt(argc, argv, ":p:hk:q:")) != -1) {
        switch (c) {
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
            return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (showhelp == TRUE)
        return help_usage("show");

    i = optind;
    do {
        args.id = argv[i];

        if ((status = tman_task_show(ctx, &args, NULL)) != TMAN_OK) {
            args.id = args.id ? args.id : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
            continue;
        } else if (tman_config->usehooks == TRUE
                   && tman_hook_show(ctx, tman_config->hooks, &args,
                                     "show") == NULL) {
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
            continue;
        }

        if (key != NULL) {
            if (show_key(ctx, key) != TMAN_OK && quiet == FALSE) {
                if (quiet == FALSE)
                    elog(1, errfmt, args.id, "key not found");
            }
        } else {
            pretty_show(ctx, key);
        }
        ctx->units.pgn = tman_hook_show_free(ctx, &args);
    } while (++i < argc);

    return status;
}
