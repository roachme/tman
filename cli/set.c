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
    args.project = args.board = args.taskid = NULL;
    atleast_one_key_set = FALSE;
    while ((c = getopt(argc, argv, ":b:d:p:qt:P:")) != -1) {
        // TODO: add a protection for duplicates, use map data structure
        switch (c) {
        case 'b':
            args.board = optarg;
            break;
        case 'p':
            args.project = optarg;
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

    if ((status = check_arg_project(&args, errfmt, quiet)))
        return status;
    else if ((status = check_arg_board(&args, errfmt, quiet)))
        return status;

    i = optind;
    do {
        args.taskid = argv[i];

        if ((status = check_arg_task(&args, errfmt, quiet)))
            continue;
        else if ((status = tman_task_set(tmancfg.base.task, &args, ctx))) {
            args.taskid = args.taskid ? args.taskid : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.taskid, tman_strerror(status));
            continue;
        } else if (hook_action(&args, "set")) {
            if (quiet == FALSE)
                elog(1, errfmt, args.taskid, "failed to execute hooks");
        }
        ctx->units = tman_unit_free(ctx->units);
    } while (++i < argc);
    return status;
}
