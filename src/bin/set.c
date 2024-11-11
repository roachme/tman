#include <string.h>

#include "../core/core.h"
#include "../core/common.h"

int tman_set(int argc, char **argv)
{
    char c;
    int idx;
    char *env = NULL, *id = NULL;
    struct unit unit = {};

    /*
Options:
    -i      set task id
    -d      set task description
    -p      set task priority. Values: [highest|high|mid|low|lowest]
    -s      set task status.
    -t      set task type. Values: [bugfix|hotfix|feature]
    */
    while ((c = getopt(argc, argv, "d:i:p:t:")) != -1) {
        switch (c) {
            case 'i':
                idx = 0;
                ++unit.size;
                if (unit.pair[idx].isset) break;
                strcpy(unit.pair[idx].key, "id");
                strcpy(unit.pair[idx].val, optarg);
                unit.pair[idx].isset = 1;
                break;
            case 'p':
                idx = 1;
                ++unit.size;
                if (unit.pair[idx].isset) break;
                strcpy(unit.pair[idx].key, "prio");
                strcpy(unit.pair[idx].val, optarg);
                unit.pair[idx].isset = 1;
                break;
            case 't':
                idx = 2;
                ++unit.size;
                if (unit.pair[idx].isset) break;
                strcpy(unit.pair[idx].key, "type");
                strcpy(unit.pair[idx].val, optarg);
                unit.pair[idx].isset = 1;
                break;
            case 'd':
                idx = 4;
                ++unit.size;
                if (unit.pair[idx].isset) break;
                strcpy(unit.pair[idx].key, "desc");
                strcpy(unit.pair[idx].val, optarg);
                unit.pair[idx].isset = 1;
                break;
        }
    }

    if (unit.size == 0)
        return elog(1, "gotta supply one of the options");

    if (optind == argc) {
        core_id_set(env, id, &unit);
        return 1;
    }

    for (int i = optind; i < argc; ++i) {
        id = argv[i];
        core_id_set(env, id, &unit);
    }
    return 1;
}

