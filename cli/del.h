#ifndef DEL_H
#define DEL_H

enum tman_del_errno {
    TMAN_DEL_NOID = 20,     /* task ID does not exist */
};

struct tman_del_opt {
    char *env;      /* environment to add a task to */
    int force;      /* ignore existent task ID */
    int help;       /* Show command usage */
};

int tman_del(int argc, char **argv);

#endif
