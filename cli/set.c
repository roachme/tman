#include <string.h>

#include "set.h"
#include "cli.h"

static const char *errfmt = "cannot set units '%s': %s";

// TODO: Find a good error message in case option fails.  */
int tman_cli_set(int argc, char **argv, struct tman_context *ctx)
{
    char c;
    int i, idx, status;
    struct tman_args args;
    int atleast_one_key_set;
    struct unit units[NKEYS] = { 0 };
    struct tman_id_set_opt opt;

    /*
       Options:
       -d      set task description
       -p      set task priority. Values: [highest|high|mid|low|lowest]
       -s      set task status.
       -t      set task type. Values: [bugfix|hotfix|feature]
     */
    args.id = args.prj = NULL;
    atleast_one_key_set = FALSE;
    while ((c = getopt(argc, argv, ":d:p:P:t:")) != -1) {
        switch (c) {
        case 'P':
            elog(1, "only current project is supported rn");
            return 1;
            break;;
        case 'p':
            idx = 0;
            if (units[idx].isset)
                break;
            atleast_one_key_set = TRUE;
            strcpy(units[idx].key, "prio");
            strcpy(units[idx].val, optarg);
            units[idx].isset = 1;
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
        tman_get_args(&args);

        if ((status = tman_check_arg_prj(&args))) {
            elog(status, errfmt, args.prj ? args.prj : "NOCURR",
                 tman_strerror());
            continue;
        } else if ((status = tman_check_arg_id(&args))) {
            elog(status, errfmt, args.id ? args.id : "NOCURR", tman_strerror());
            continue;
        }

        status = tman_id_set(ctx, &args, units, &opt);
    } while (++i < argc);

    return status;
}
