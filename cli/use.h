#ifndef TMAN_USE_H
#define TMAN_USE_H

enum tman_cli_use_errno {
    TMAN_USE_IDREQ = 20,     /* task ID required */
    TMAN_USE_NOID  = 21,     /* task ID does not exist */
};

struct tman_cli_use_opt {
    char *prj;
    int force;
    int help;
};

#endif
