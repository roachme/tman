#include <string.h>

#include "../core.h"
#include "../common.h"

int tman_set(int argc, char **argv)
{
    char c;
    int idx;
    char *env = NULL, *id = NULL;
    struct bunit bunit = {};

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
                ++bunit.size;
                if (bunit.pair[idx].isset) break;
                strcpy(bunit.pair[idx].key, "id");
                strcpy(bunit.pair[idx].val, optarg);
                bunit.pair[idx].isset = 1;
                break;
            case 'p':
                idx = 1;
                ++bunit.size;
                if (bunit.pair[idx].isset) break;
                strcpy(bunit.pair[idx].key, "prio");
                strcpy(bunit.pair[idx].val, optarg);
                bunit.pair[idx].isset = 1;
                break;
            case 't':
                idx = 2;
                ++bunit.size;
                if (bunit.pair[idx].isset) break;
                strcpy(bunit.pair[idx].key, "type");
                strcpy(bunit.pair[idx].val, optarg);
                bunit.pair[idx].isset = 1;
                break;
            case 'd':
                idx = 4;
                ++bunit.size;
                if (bunit.pair[idx].isset) break;
                strcpy(bunit.pair[idx].key, "desc");
                strcpy(bunit.pair[idx].val, optarg);
                bunit.pair[idx].isset = 1;
                break;
        }
    }

    if (bunit.size == 0)
        return elog(1, "gotta supply one of the options");

    if (optind == argc) {
        core_id_set(env, id, &bunit);
        return 1;
    }

    for (int i = optind; i < argc; ++i) {
        id = argv[i];
        core_id_set(env, id, &bunit);
    }
    return 1;
}

