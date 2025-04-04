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
    int i, status;
    struct tman_arg args;

    key = args.prj = args.id = args.brd = NULL;
    while ((c = getopt(argc, argv, ":p:hk:")) != -1) {
        switch (c) {
        case 'p':
            args.prj = optarg;
            break;
        case 'k':
            key = optarg;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `%c'", optopt);
        }
    }

    i = optind;
    do {
        args.id = argv[i];
        tman_get_args(&args);

        if ((status = tman_check_arg_prj(&args))) {
            elog(status, errfmt, args.prj ? args.prj : "NOCURR",
                 tman_strerror());
            continue;
        } else if ((status = tman_check_arg_id(&args))) {
            elog(status, errfmt, args.id ? args.id : "NOCURR", tman_strerror());
            continue;
        }

        if ((status = tman_id_show(ctx, &args, NULL)) != TMAN_OK) {
            elog(status, errfmt, args.id, tman_strerror());
            continue;
        }
        if (tman_config->usehooks == TRUE
            && tman_hook_show(ctx, tman_config->hooks, &args, "show") == NULL)
            elog(status, errfmt, args.id, tman_strerror());

        if (key != NULL) {
            if (show_key(ctx, key) != TMAN_OK)
                elog(1, errfmt, args.id, "key not found");
        } else {
            pretty_show(ctx, key);
        }
        ctx->units.pgn = tman_hook_show_free(ctx, &args);
    } while (++i < argc);

    return status;
}
