#include <string.h>

#include "set.h"
#include "cli.h"

// TODO: Find a good error message in case option fails.  */
int tman_cli_set(int argc, char **argv, tman_ctx_t *ctx)
{
    char c;
    int idx;
    int atleast_one_key_set;
    char *env = NULL, *id = NULL;
    struct unit units[NKEYS] = { 0 };
    struct tman_id_set_opt opt;

    /*
Options:
    -i      set task id
    -d      set task description
    -p      set task priority. Values: [highest|high|mid|low|lowest]
    -s      set task status.
    -t      set task type. Values: [bugfix|hotfix|feature]
    */
    atleast_one_key_set = FALSE;
    while ((c = getopt(argc, argv, ":d:i:p:t:")) != -1) {
        switch (c) {
            /*
            case 'i':
                idx = 0;
                if (units[idx].isset) break;
                atleast_one_key_set = TRUE;
                strcpy(units[idx].key, "id");
                strcpy(units[idx].val, optarg);
                units[idx].isset = 1;
                break;
                */
            case 'p':
                idx = 0;
                if (units[idx].isset) break;
                atleast_one_key_set = TRUE;
                strcpy(units[idx].key, "prio");
                strcpy(units[idx].val, optarg);
                units[idx].isset = 1;
                break;
            case 't':
                idx = 1;
                if (units[idx].isset) break;
                atleast_one_key_set = TRUE;
                strcpy(units[idx].key, "type");
                strcpy(units[idx].val, optarg);
                units[idx].isset = 1;
                break;
            case 'd':
                idx = 3;
                if (units[idx].isset) break;
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

    if (optind == argc) {
        tman_id_set(ctx, env, id, units, &opt);
    }
    for (int i = optind; i < argc; ++i) {
        tman_id_set(ctx, env, argv[i], units, &opt);
    }
    return 1;
}
