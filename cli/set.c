#include <ctype.h>
#include <string.h>

#include "cli.h"
#include "aux/toggle.h"
#include "aux/config.h"

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
int tman_cli_set(int argc, char **argv, tman_ctx_t * ctx)
{
    char c;
    int i, quiet, status;
    tman_arg_t args;
    int atleast_one_key_set;
    const char *errfmt = "cannot set task units '%s': %s";

    quiet = FALSE;
    args.prj = args.brd = args.task = NULL;
    atleast_one_key_set = FALSE;
    while ((c = getopt(argc, argv, ":b:d:p:qt:P:")) != -1) {
        // TODO: add a protection for duplicates, use map data structure
        switch (c) {
        case 'b':
            args.brd = optarg;
            break;
        case 'p':
            args.prj = optarg;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case 't':
            if (valid_type(optarg) == FALSE) {
                elog(1, "invalid priority '%s'", optarg);
                help_usage("set");
                return 1;
            }
            atleast_one_key_set = TRUE;
            ctx->units = tman_unit_add(ctx->units, "type", optarg);
            break;
        case 'd':
            if (valid_desc(optarg) == FALSE) {
                elog(1, "invalid description '%s'", optarg);
                help_usage("set");
                return 1;
            }
            atleast_one_key_set = TRUE;
            ctx->units = tman_unit_add(ctx->units, "desc", optarg);
            break;
        case 'P':
            if (valid_prio(optarg) == FALSE) {
                elog(1, "invalid priority '%s'", optarg);
                help_usage("set");
                return 1;
            }
            atleast_one_key_set = TRUE;
            ctx->units = tman_unit_add(ctx->units, "prio", optarg);
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

    if ((status = toggle_prj_get_curr(tmancfg->base.task, &args))) {
        return status;
    }
    if ((status = toggle_brd_get_curr(tmancfg->base.task, &args))) {
        return status;
    }

    i = optind;
    do {
        args.task = argv[i];

        if ((status = toggle_task_get_curr(tmancfg->base.task, &args))) {
            continue;
        } else if ((status = tman_task_set(ctx, &args)) != LIBTMAN_OK) {
            args.task = args.task ? args.task : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.task, tman_strerror());
            continue;
        } else if (hook_action(&args, "set")) {
            if (quiet == FALSE)
                elog(1, errfmt, args.task, "failed to execute hooks");
        }
        ctx->units = tman_unit_free(ctx->units);
    } while (++i < argc);
    return status;
}
