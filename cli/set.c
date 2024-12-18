#include <string.h>
#include "../src/tman.h"

int tman_set(int argc, char **argv)
{
    char c;
    int idx;
    int atleast_one_key_set;
    char *env = NULL, *id = NULL;
    struct unitbin units[NKEYS] = { 0 };

    /*
Options:
    -i      set task id
    -d      set task description
    -p      set task priority. Values: [highest|high|mid|low|lowest]
    -s      set task status.
    -t      set task type. Values: [bugfix|hotfix|feature]
    */
    while ((c = getopt(argc, argv, ":d:i:p:t:")) != -1) {
        switch (c) {
            case 'i':
                idx = 0;
                if (units[idx].isset) break;
                atleast_one_key_set = TRUE;
                strcpy(units[idx].key, "id");
                strcpy(units[idx].val, optarg);
                units[idx].isset = 1;
                break;
            case 'p':
                idx = 1;
                if (units[idx].isset) break;
                atleast_one_key_set = TRUE;
                strcpy(units[idx].key, "prio");
                strcpy(units[idx].val, optarg);
                units[idx].isset = 1;
                break;
            case 't':
                idx = 2;
                if (units[idx].isset) break;
                atleast_one_key_set = TRUE;
                strcpy(units[idx].key, "type");
                strcpy(units[idx].val, optarg);
                units[idx].isset = 1;
                break;
            case 'd':
                idx = 4;
                if (units[idx].isset) break;
                atleast_one_key_set = TRUE;
                strcpy(units[idx].key, "desc");
                strcpy(units[idx].val, optarg);
                units[idx].isset = 1;
                break;
            case ':':
                return elog(TMAN_INVOPT, "option `-%c' requires an argument", optopt);
            default:
                return elog(TMAN_INVOPT, "invalid option `%c'", optopt);
        }
    }

    if (atleast_one_key_set == FALSE)
        return elog(1, "gotta supply one of the options");

    if (optind == argc) {
        core_id_set(env, id, units);
    }
    for (int i = optind; i < argc; ++i) {
        core_id_set(env, argv[i], units);
    }
    return 1;
}
