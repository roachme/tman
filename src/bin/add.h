/*
    Add a task to a specified or current environment.
    If multiple task IDs are passed then switch to last
    successfully added task ID.
*/

#ifndef TMAN_ADD_H
#define TMAN_ADD_H

enum tman_add_errno {
    TMAN_ADD_IDEXT = 3,     /* task ID already exists */
    TMAN_ADD_IDREQ = 4,     /* task ID required */
};

struct tman_add_opt {
    char *env;      /* environment to add a task to */
    int force;      /* ignore existent task ID */
    int help;       /* Show command usage */
};

int tman_add_usage(void);
int tman_add(int argc, char **argv);

#endif
