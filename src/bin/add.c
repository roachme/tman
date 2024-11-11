/*
    Add a task to specified or current environment.
*/

#include "add.h"
#include "../core/core.h"
#include "../core/common.h"

int tman_add(int argc, char **argv)
{
    char c;
    char *env = NULL;
    int retcode;

    // TODO: Add force flag to suppress errors
    while ((c = getopt(argc, argv, "e:f")) != -1) {
        switch (c) {
            case 'e':
                env = optarg; break ;
        }
    }

    if (optind == argc)
        return elog("task id required");

    /*
         // From GNU rm (in the end)
         enum RM_status status = rm (file, &x);
         affirm (VALID_STATUS (status));
         return status == RM_ERROR ? EXIT_FAILURE : EXIT_SUCCESS;
    */
    for (int i = optind; i < argc; ++i)
        retcode = core_id_add(env, argv[i]);
    return retcode == 0 ? core_currdir() : 1;
}
