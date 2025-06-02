#include "cli.h"
#include "cli.h"
#include "help.h"
#include <string.h>
#include <ctype.h>

// "prio",  /* task priority */
// "type",  /* task type: bugfix, hotfix, feature */
// "date",  /* task date of creation */
// "desc",  /* task description */
// "user"   /* who created, who's woring on it */
// "users"  /* list of users */
// "teams"  /* list of teams */
// "label"  /* list of labels */
// "time"   /* time tracker */

/* roachme: replace all prios if user specifies any in config file */
static int valid_prio(const char *val)
{
    char *prios[] = { "lowest", "low", "mid", "high", "highest" };
    int size = sizeof(prios) / sizeof(prios[0]);

    for (int i = 0; i < size; ++i)
        if (strncmp(val, prios[i], 10) == 0)
            return TRUE;
    return FALSE;
}

/* roachme: replace all types if user specifies any in config file */
static int valid_type(const char *val)
{
    char *types[] = { "task", "bugfix", "feature", "hotfix" };
    int size = sizeof(types) / sizeof(types[0]);

    for (int i = 0; i < size; ++i) {
        if (strncmp(val, types[i], 10) == 0)
            return TRUE;
    }
    return FALSE;
}

static int valid_desc(const char *val)
{
    if (!isalnum(*val++))
        return FALSE;
    for (; *val; ++val)
        if (!(isalnum(*val) || isspace(*val) || *val == '_' || *val == '-'))
            return FALSE;
    return isalnum(*--val) != 0;
}

// TODO: Find a good error message in case option fails.  */
int tman_cli_set(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    int i, status;
    struct tman_arg args;
    int atleast_one_key_set;
    struct tman_option opt;
    const char *errfmt = "cannot set task units '%s': %s";

    args.id = args.prj = NULL;
    atleast_one_key_set = FALSE;
    while ((c = getopt(argc, argv, ":d:p:t:P:")) != -1) {
        // TODO: add a protection for duplicates, use map data structure
        switch (c) {
        case 'p':
            args.prj = optarg;
            break;
        case 't':
            if (valid_type(optarg) == FALSE) {
                elog(1, "invalid priority '%s'", optarg);
                help_usage("set");
                return 1;
            }
            atleast_one_key_set = TRUE;
            ctx->unitbin = tman_unit_add(ctx->unitbin, "type", optarg);
            break;
        case 'd':
            if (valid_desc(optarg) == FALSE) {
                elog(1, "invalid description '%s'", optarg);
                help_usage("set");
                return 1;
            }
            atleast_one_key_set = TRUE;
            ctx->unitbin = tman_unit_add(ctx->unitbin, "desc", optarg);
            break;
        case 'P':
            if (valid_prio(optarg) == FALSE) {
                elog(1, "invalid priority '%s'", optarg);
                help_usage("set");
                return 1;
            }
            atleast_one_key_set = TRUE;
            ctx->unitbin = tman_unit_add(ctx->unitbin, "prio", optarg);
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `-%c'", optopt);
        }
    }

    if (atleast_one_key_set == FALSE) {
        elog(1, "gotta supply one of the options");
        help_usage("set");
        return 1;
    }

    i = optind;
    do {
        args.id = argv[i];
        if ((status =
             tman_task_set(ctx, &args, ctx->unitbin, &opt)) != LIBTMAN_OK) {
            args.id = args.id ? args.id : "NOCURR";
            elog(status, errfmt, args.id, tman_strerror());
            return status;
        }
        // TODO: add support for hooks (if needed)
    } while (++i < argc);

    ctx->unitbin = tman_unit_free(ctx, &args, NULL);
    return status;
}
