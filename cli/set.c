#include <string.h>

#include "cli.h"
#include "config.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_set(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    int i, idx, quiet, status;
    struct tman_arg args;
    int atleast_one_key_set;
    struct unit units[NKEYS] = { 0 };
    struct tman_option opt;
    const char *errfmt = "cannot show units '%s': %s";

    /*
       Options:
       -d      set task description
       -p      set task priority. Values: [highest|high|mid|low|lowest]
       -s      set task status.
       -t      set task type. Values: [bugfix|hotfix|feature]
     */
    args.id = args.prj = NULL;
    quiet = atleast_one_key_set = FALSE;
    while ((c = getopt(argc, argv, ":d:hp:qt:P:")) != -1) {
        switch (c) {
        case 'p':
            args.prj = optarg;
            break;
        case 'q':
            quiet = TRUE;
            break;
        case 't':
            idx = 1;
            if (units[idx].isset)
                break;
            atleast_one_key_set = TRUE;
            strcpy(units[idx].key, "type");
            strcpy(units[idx].val, optarg);
            units[idx].isset = 1;
            break;
        case 'd':
            idx = 3;
            if (units[idx].isset)
                break;
            atleast_one_key_set = TRUE;
            strcpy(units[idx].key, "desc");
            strcpy(units[idx].val, optarg);
            units[idx].isset = 1;
            break;
        case 'P':
            idx = 0;
            if (units[idx].isset)
                break;
            atleast_one_key_set = TRUE;
            strcpy(units[idx].key, "prio");
            strcpy(units[idx].val, optarg);
            units[idx].isset = 1;
            break;
        case ':':
            return elog(1, "option `-%c' requires an argument", optopt);
        default:
            return elog(1, "invalid option `%c'", optopt);
        }
    }

    if (atleast_one_key_set == FALSE)
        return elog(1, "gotta supply one of the options");

    i = optind;
    do {
        args.id = argv[i];
        if ((status = tman_task_set(ctx, &args, units, &opt)) != LIBTMAN_OK) {
            args.id = args.id ? args.id : "NOCURR";
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
            return status;
        } else
            if ((status =
                 tman_hook_action(ctx, tman_config->hooks, &args,
                                  "set")) != LIBTMAN_OK) {
            if (quiet == FALSE)
                elog(status, errfmt, args.id, tman_strerror());
            return status;
        }

    } while (++i < argc);

    return status;
}
